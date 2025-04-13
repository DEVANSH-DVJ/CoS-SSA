#include "../headers.hh"

#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>

FILE* output_file;
void set_llvm_out(FILE* file) {
  output_file = file;
}

extern std::unique_ptr<llvm::Module> module;
extern Program* program;

void deconstruct_context_transition(llvm::CallInst* call, std::map<int, int>& transitions) {
  CHECK_INVARIANT(call != nullptr, "Expected a non null call inst");

}

void deconstruct_metamorphic_assign(std::map<int, SSA_Meta*>* metas, llvm::Value* value,
                                    std::map<llvm::GlobalVariable*, llvm::StoreInst*>& defs,
                                    std::map<llvm::GlobalVariable*, std::set<llvm::GlobalVariable*>>& uses) {
  // TODO: do not handle PHIs here, handle them in a later pass
  // Do handle context here

  for (auto pair : *metas) {
    std::list<SSA_Stmt*>* stmts = pair.second->get_stmts();
    CHECK_INVARIANT(stmts->size() > 0, "Expected at least one statement for each metamorphic assignment");
    // All other statements should be phi statements, so we can handle them during a later pass
    SSA_Stmt* final_stmt = *--stmts->end();
    // Add dep
  }

  // TODO: remove value
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
  std::map<QDef, llvm::GlobalVariable*> qdef_globals;

  std::map<llvm::GlobalVariable*, llvm::StoreInst*> defs;
  std::map<llvm::GlobalVariable*, std::set<llvm::GlobalVariable*>> uses;

  for (auto pair : *program->get_procs()) {
    for (int node : pair.second->get_ssa_nodes()) {
      SSA_Node* ssa_node = program->get_ssa_node(node, true);
      llvm::Value* value = program->get_llvm_node(node, true);
      // Possible combinations (treating the ssa node as a cfg node):
      // global loads: global variable loaded (or GEP)
      // global stores: global variable stored

      // Assign node with 2 ops: store has an instruction operand; the instruction is add, sub, mul, div, ... (frankly the instruction does not matter and we can replace it always using some gen_rhs function)
      // Otherwise, we have a call that is a "global" store
      // Usevars are always global loads

      if (value == nullptr) {
        continue;
      }

      if (ssa_node->get_type() == SSA_NodeType::SSA_CallNode) {
        deconstruct_context_transition(llvm::dyn_cast<llvm::CallInst>(value), program->get_ddg_transitions(ssa_node->get_node_id()));
        continue;
      }

      CHECK_INVARIANT(ssa_node->get_type() == SSA_NodeType::SSA_AssignNode, "Expected assign node");

      // If this is a usevar, the corresponding instruction is a load
      // Else this is a store instruction
      std::map<int, SSA_Meta*>* metas = ssa_node->get_metas();
      deconstruct_metamorphic_assign(metas, value, defs, uses);
    }
  }

  deconstruct_phi_nodes(defs, uses);
}
