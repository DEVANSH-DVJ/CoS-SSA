#include "../headers.hh"

#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstrTypes.h>

FILE* output_file;
void set_llvm_out(FILE* file) {
  output_file = file;
}

extern std::unique_ptr<llvm::Module> module;
extern Program* program;

constexpr const char* CUR_CONTEXT_NAME = "__cos_ssa_current_context";

void set_context_to(llvm::GlobalVariable* cur_context, int new_context, llvm::Instruction* insert_before) {
  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  new llvm::StoreInst(llvm::ConstantInt::get(int_type, new_context), cur_context, insert_before);
}

void deconstruct_context_transition(llvm::CallInst* call, llvm::GlobalVariable* cur_context, std::map<int, int>& transitions) {
  CHECK_INVARIANT(call != nullptr, "Expected a non null call inst");
  CHECK_INVARIANT(transitions.size() > 0, "Expected at least one context transition");

  if (transitions.size() == 1) {
    set_context_to(cur_context, transitions.begin()->second, call);
    return;
  }

  llvm::BasicBlock* callBB = call->getParent();
  llvm::BasicBlock* chainBB = callBB->splitBasicBlockBefore(call);
  chainBB->getTerminator()->eraseFromParent();
  llvm::IRBuilder<> builder (chainBB);

  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  llvm::Value* cur_context_value = builder.CreateLoad(int_type, cur_context);

  auto final_transition = --transitions.end();
  for (auto it = transitions.begin(); it != final_transition; ++it) {
    llvm::Value* cond = builder.CreateICmpEQ(cur_context_value, llvm::ConstantInt::get(int_type, it->first));

    llvm::BasicBlock* trueBB = llvm::BasicBlock::Create(module->getContext(), "true");
    llvm::BasicBlock* falseBB = llvm::BasicBlock::Create(module->getContext(), "false");
    builder.CreateCondBr(cond, trueBB, falseBB);

    builder.SetInsertPoint(trueBB);
    llvm::Instruction* br = builder.CreateBr(callBB);
    set_context_to(cur_context, it->second, br);
    
    chainBB = falseBB;
  }

  llvm::Instruction* br = builder.CreateBr(callBB);
  set_context_to(cur_context, final_transition->second, br);
}

llvm::GlobalVariable* get_global(SSA_Opd* operand, std::map<std::string, llvm::GlobalVariable*>& qdef_globals) {
  std::string qdef = operand->str();
  auto it = qdef_globals.find(qdef);
  if (it == qdef_globals.end()) {
    llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
    llvm::GlobalVariable* global = new llvm::GlobalVariable(int_type, false, llvm::GlobalValue::InternalLinkage,
                                                            llvm::ConstantInt::get(int_type, 0), qdef);
    qdef_globals[qdef] = global;
    return global;
  }
  return it->second;
}

llvm::Value* get_value(SSA_Opd* operand, llvm::Instruction* insert_before,
                       std::map<std::string, llvm::GlobalVariable*>& qdef_globals) {
  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  switch (operand->get_type()) {
    case SSA_VarOpd: {
      return new llvm::LoadInst(int_type, get_global(operand, qdef_globals), "", insert_before);
    }
    case SSA_NumOpd:
      return llvm::ConstantInt::get(int_type, operand->get_opd_value());
    default:
      CHECK_INVARIANT(false, "Expected a variable or number operand");
      return nullptr;
  }
}

void create_assignment(std::list<SSA_Stmt*>* stmts, llvm::Instruction* insert_before,
                       std::map<std::string, llvm::GlobalVariable*>& qdef_globals,
                       std::map<llvm::GlobalVariable*, llvm::StoreInst*>& defs,
                       std::map<llvm::GlobalVariable*, std::set<llvm::GlobalVariable*>>& uses) {
  CHECK_INVARIANT(stmts->size() > 0, "Expected at least one statement for each metamorphic assignment");
  auto final_stmt = --stmts->end();
  CHECK_INVARIANT((*final_stmt)->get_type() == SSA_AssignStmt, "Expected final stmt to be an assign stmt");
  for (auto it = stmts->begin(); it != final_stmt; ++it) {
    // PHI nodes will be deconstructed in a second pass
    CHECK_INVARIANT((*it)->get_type() == SSA_PhiStmt, "Expected PHI stmt");
    llvm::GlobalVariable* lhs = get_global((*it)->get_lhs(), qdef_globals);
    for (SSA_Opd* use : *(*it)->get_phi_uses()) {
      llvm::GlobalVariable* rhs = get_global(use, qdef_globals);
      uses[rhs].insert(lhs);
    }
  }

  std::vector<SSA_Opd*> operands = (*final_stmt)->get_rhs();
  std::string op = (*final_stmt)->get_op();
  llvm::Value* stored_value;
  if (op == "=") {
    CHECK_INVARIANT(operands.size() == 1, "Expected 1 operand");
    stored_value = get_value(operands[0], insert_before, qdef_globals);
  } else {
    llvm::Value* v1 = get_value(operands[0], insert_before, qdef_globals);
    llvm::Value* v2 = get_value(operands[1], insert_before, qdef_globals);
    if (op == "+") {
      stored_value = llvm::BinaryOperator::CreateAdd(v1, v2);
    } else if (op == "-") {
      stored_value = llvm::BinaryOperator::CreateSub(v1, v2);
    } else if (op == "*") {
      stored_value = llvm::BinaryOperator::CreateMul(v1, v2);
    } else if (op == "/") {
      stored_value = llvm::BinaryOperator::CreateSDiv(v1, v2);
    } else {
      CHECK_INVARIANT(false, "Control should not reach");
    }
  }

  llvm::GlobalVariable* store_loc = get_global((*final_stmt)->get_lhs(), qdef_globals);
  llvm::StoreInst* store = new llvm::StoreInst(stored_value, store_loc, insert_before);
  defs[llvm::dyn_cast<llvm::GlobalVariable>(store_loc)] = store;
}

void deconstruct_metamorphic_assign(std::map<int, SSA_Meta*>* metas, llvm::Instruction* assign, llvm::GlobalVariable* cur_context,
                                    std::map<std::string, llvm::GlobalVariable*>& qdef_globals,
                                    std::map<llvm::GlobalVariable*, llvm::StoreInst*>& defs,
                                    std::map<llvm::GlobalVariable*, std::set<llvm::GlobalVariable*>>& uses) {
  CHECK_INVARIANT(assign != nullptr, "Expected a non null call inst");
  CHECK_INVARIANT(metas->size() > 0, "Expected at least one meta assignment");

  if (metas->size() == 1) {
    create_assignment(metas->begin()->second->get_stmts(), assign, qdef_globals, defs, uses);
    assign->eraseFromParent();
    return;
  }

  llvm::BasicBlock* assignBB = assign->getParent();
  llvm::BasicBlock* chainBB = assignBB->splitBasicBlockBefore(assign);
  chainBB->getTerminator()->eraseFromParent();
  llvm::IRBuilder<> builder (chainBB);

  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  llvm::Value* cur_context_value = builder.CreateLoad(int_type, cur_context);

  auto final_meta = --metas->end();
  for (auto it = metas->begin(); it != final_meta; ++it) {
    llvm::Value* cond = builder.CreateICmpEQ(cur_context_value, llvm::ConstantInt::get(int_type, it->second->get_meta_num().second));

    llvm::BasicBlock* trueBB = llvm::BasicBlock::Create(module->getContext(), "true");
    llvm::BasicBlock* falseBB = llvm::BasicBlock::Create(module->getContext(), "false");
    builder.CreateCondBr(cond, trueBB, falseBB);

    builder.SetInsertPoint(trueBB);
    llvm::Instruction* br = builder.CreateBr(assignBB);

    create_assignment(it->second->get_stmts(), br, qdef_globals, defs, uses);

    chainBB = falseBB;
  }

  llvm::Instruction* br = builder.CreateBr(assignBB);
  create_assignment(final_meta->second->get_stmts(), br, qdef_globals, defs, uses);

  assign->eraseFromParent();
}

void deconstruct_phi_nodes(std::map<llvm::GlobalVariable*, llvm::StoreInst*>& defs,
                           std::map<llvm::GlobalVariable*, std::set<llvm::GlobalVariable*>>& uses) {
  for (auto pair : uses) {
    auto it = defs.find(pair.first);
    if (it == defs.end()) {
      // All phi node uses depend on the default value, so set their default value to be the same as this
      CHECK_INVARIANT(pair.first->getNumOperands() == 1, "Expected a global variable with a single operand");
      llvm::Value* default_value = pair.first->getOperand(0);
      for (llvm::GlobalVariable* use : pair.second) {
        use->setOperand(0, default_value);
      }
      continue;
    }

    llvm::StoreInst* store = it->second;
    for (llvm::GlobalVariable* use : pair.second) {
      new llvm::StoreInst(store->getValueOperand(), use, store);
    }
  }
}

void deconstruct_ssa() {
  std::map<std::string, llvm::GlobalVariable*> qdef_globals;
  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  llvm::GlobalVariable* cur_context = new llvm::GlobalVariable(int_type, false, llvm::GlobalValue::InternalLinkage,
                                                           llvm::ConstantInt::get(int_type, 0), CUR_CONTEXT_NAME);

  std::map<llvm::GlobalVariable*, llvm::StoreInst*> defs;
  std::map<llvm::GlobalVariable*, std::set<llvm::GlobalVariable*>> uses;

  for (auto pair : *program->get_procs()) {
    for (int node : pair.second->get_ssa_nodes()) {
      SSA_Node* ssa_node = program->get_ssa_node(node, true);
      llvm::Value* value = program->get_llvm_node(node, true);
      if (value == nullptr) {
        continue;
      }

      if (ssa_node->get_type() == SSA_NodeType::SSA_CallNode) {
        deconstruct_context_transition(llvm::dyn_cast<llvm::CallInst>(value), cur_context, program->get_ddg_transitions(ssa_node->get_node_id()));
        continue;
      }

      CHECK_INVARIANT(ssa_node->get_type() == SSA_NodeType::SSA_AssignNode, "Expected assign node");
      std::map<int, SSA_Meta*>* metas = ssa_node->get_metas();
      deconstruct_metamorphic_assign(metas, llvm::dyn_cast<llvm::Instruction>(value), cur_context, qdef_globals, defs, uses);
    }
  }

  deconstruct_phi_nodes(defs, uses);
}
