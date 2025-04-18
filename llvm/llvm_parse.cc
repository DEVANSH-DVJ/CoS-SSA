#include "../headers.hh"

#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>

#include <memory>
#include <map>
#include <set>
#include <queue>

extern Program* program;
std::string ll_file;

llvm::Module* module = nullptr;

void llvm_set_in(llvm::Module* llvm_module) {
  module = llvm_module;
}

int node_num = 1;

std::string func_ret_to_global(llvm::Function* func) {
  return "$" + func->getName().str() + "_ret";
}

bool is_usable_func(llvm::Function* func) {
  // Get other generated code (like templates), use return !func->isDeclaration();
  /*return !func->isDeclaration();*/
  return !(func->isDeclaration() || func->getName().starts_with("_Z") || func->getName().starts_with("__") || func->getName().starts_with("_GLOBAL__"));
}

std::map<llvm::BasicBlock*, std::set<llvm::BasicBlock*>> construct_llvm_cfg(llvm::Function* func) {
  std::map<llvm::BasicBlock*, std::set<llvm::BasicBlock*>> cfg_transitions;
  std::queue<llvm::BasicBlock*> worklist;
  worklist.push(&func->getEntryBlock());
  cfg_transitions[&func->getEntryBlock()] = std::set<llvm::BasicBlock*>();

  while (!worklist.empty()) {
    llvm::BasicBlock* bb = worklist.front();
    worklist.pop();

    for (llvm::BasicBlock* successor : llvm::successors(bb)) {
      cfg_transitions[bb].insert(successor);

      if (cfg_transitions.find(successor) == cfg_transitions.end()) {
        worklist.push(successor);
        cfg_transitions[successor] = std::set<llvm::BasicBlock*>();
      }
    }
  }

  return cfg_transitions;
}

typedef struct GlobalInfo {
  std::set<llvm::GlobalVariable*> globals;
  std::map<llvm::Instruction*, std::string> loads;
  std::map<llvm::Instruction*, std::string> stores;
  std::set<std::string> functions;
} GlobalInfo;

GlobalInfo get_globals(llvm::Module* module) {
  std::set<std::string> addr_taken;
  // Probably a better way to check if a global's addr is taken
  for (llvm::GlobalVariable& var : module->globals()) {
    if (var.getNumOperands() != 1) {
      addr_taken.insert(var.getName().str());
      break;
    }
    bool found = false;
    for (llvm::Use& use : var.uses()) {
      llvm::User* user = use.getUser();
      if (llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(user)) {
        if (&var != load->getPointerOperand()) {
          found = true;
          break;
        }
      } else if (llvm::StoreInst* load = llvm::dyn_cast<llvm::StoreInst>(user)) {
        if (&var != load->getPointerOperand()) {
          found = true;
          break;
        }
      } else {
        found = true;
        break;
      }
    }
    if (found) {
      addr_taken.insert(var.getName().str());
      break;
    }
  }

  GlobalInfo globals;
  for (llvm::Function& func : *module) {
    if (!is_usable_func(&func)) {
      continue;
    }

    globals.functions.insert(func.getName().str());
    for (llvm::BasicBlock& bb : func) {
      for (llvm::Instruction& inst : bb) {
        if (inst.isVolatile()) {
          continue;
        }
        if (llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(&inst)) {
          if (llvm::GlobalVariable* var = llvm::dyn_cast<llvm::GlobalVariable>(load->getPointerOperand())) {
            if (addr_taken.find(var->getName().str()) == addr_taken.end() && var->getNumOperands() == 1) {
              globals.globals.insert(var);
              globals.loads[&inst] = var->getName();
            }
          }
        } else if (llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(&inst)) {
          if (llvm::GlobalVariable* var = llvm::dyn_cast<llvm::GlobalVariable>(store->getPointerOperand())) {
            if (addr_taken.find(var->getName().str()) == addr_taken.end() && var->getNumOperands() == 1) {
              globals.globals.insert(var);
              globals.stores[&inst] = var->getName();
            }
          }
        // Handle non scalar types
        /*} else if (llvm::GetElementPtrInst* gep = llvm::dyn_cast<llvm::GetElementPtrInst>(&inst)) {*/
        /*  if (llvm::GlobalVariable* var = llvm::dyn_cast<llvm::GlobalVariable>(gep->getPointerOperand())) {*/
        /*    if (addr_taken.find(var->getName().str()) == addr_taken.end()) {*/
        /*      globals.globals.insert(var);*/
        /*      globals.loads[&inst] = var->getName();*/
        /*    }*/
        /*  }*/
        // TODO: handle parameters
        // "Promote" returns to globals
        /*} else if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(&inst)) {*/
        /*  if (is_usable_func(call->getCalledFunction())) {*/
        /*    globals.loads[&inst] = func_ret_to_global(call->getCalledFunction());*/
        /*  }*/
        /*} else if (llvm::isa<llvm::ReturnInst>(&inst)) {*/
        /*  globals.stores[&inst] = func_ret_to_global(&func);*/
        }
      }
    }
  }
  return globals;
}

void erase_replaceable_operand(llvm::Value* value, std::set<llvm::Value*>& erased_uses) {
  llvm::LoadInst* load;
  if ((load = llvm::dyn_cast<llvm::LoadInst>(value)) && load->getNumUses() == 1) {
    erased_uses.insert(load);
    load->eraseFromParent();
  }
}

void erase_replaceable_inst(llvm::Instruction* inst, std::set<llvm::Value*>& erased_uses) {
  CHECK_INVARIANT(inst->getNumOperands() == 2, "Expected instruction with two operands");
  
  erase_replaceable_operand(inst->getOperand(0), erased_uses);
  erase_replaceable_operand(inst->getOperand(1), erased_uses);

  if (inst->getNumUses() == 1) {
    inst->eraseFromParent();
  }
}

bool get_operand_repr(llvm::Value* operand, CFG_Opd** repr, const GlobalInfo& globals) {
  if (llvm::Instruction* inst = llvm::dyn_cast<llvm::Instruction>(operand)) {
    auto it = globals.loads.find(inst);
    if (it != globals.loads.end()) {
      *repr = new CFG_Opd(CFG_OpdType::CFG_VarOpd, it->second);
      return true;
    }
  } else if (llvm::ConstantInt* value = llvm::dyn_cast<llvm::ConstantInt>(operand)) {
    llvm::SmallVector<char> vec;
    value->getValue().toStringSigned(vec);
    *repr = new CFG_Opd(CFG_OpdType::CFG_NumOpd, std::atoi(std::string(vec.begin(), vec.end()).c_str()));
    return true;
  }

  return false;
}

struct RHS {
  std::string op;
  CFG_Opd* ropd1;
  CFG_Opd* ropd2;
};

RHS get_assignment_value(llvm::Value* value, const GlobalInfo& globals, std::set<llvm::Value*>& erased_uses) {
  static const std::map<unsigned int, char> ops {
    {llvm::Instruction::Add, '+'},
    {llvm::Instruction::Sub, '-'},
    {llvm::Instruction::Mul, '*'},
    {llvm::Instruction::SDiv, '/'}
  };

  CFG_Opd* ropd1;
  if (get_operand_repr(value, &ropd1, globals)) {
    erase_replaceable_operand(value, erased_uses);
    return {"=", ropd1, nullptr};
  }
  if (llvm::Instruction* inst = llvm::dyn_cast<llvm::Instruction>(value)) {
    auto it = ops.find(inst->getOpcode());
    if (it != ops.end()) {
      if (inst->getNumOperands() == 2) {
        if (get_operand_repr(inst->getOperand(0), &ropd1, globals)) {
          CFG_Opd* ropd2;
          if (get_operand_repr(inst->getOperand(1), &ropd2, globals)) {
            erase_replaceable_inst(inst, erased_uses);
            return {std::string(1, it->second), ropd1, ropd2};
          }
          delete ropd1;
        }
      }
    }
  }

  return {"=", new CFG_Opd(CFG_OpdType::CFG_InputOpd), nullptr};
}

std::vector<std::pair<CFG_Node*, llvm::Value*>> get_nodes_in_basic_block(const std::string& proc, llvm::BasicBlock* bb, const GlobalInfo& globals) {
  std::vector<std::pair<CFG_Node*, llvm::Value*>> res;
  std::set<llvm::Value*> erased_uses;

  if (bb->isEntryBlock()) {
    res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_StartNode, 0, "START " + proc), nullptr));

    if (proc == "main") { // TODO: handle initial values some other way
      for (llvm::GlobalVariable* var : globals.globals) {
        CFG_Opd* ropd1;
        if (get_operand_repr(var->getOperand(0), &ropd1, globals)) {
          res.push_back(std::make_pair(
            new CFG_Node(CFG_NodeType::CFG_AssignNode, 0, "=", new CFG_Opd(CFG_OpdType::CFG_VarOpd, var->getName().str()), ropd1, nullptr),
            nullptr));
        }
      }
    }
  }

  for (llvm::Instruction& inst : *bb) {
    auto it = globals.loads.find(&inst);
    if (it != globals.loads.end()) {
      if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(&inst)) {
        auto it = globals.functions.find(call->getCalledFunction()->getName().str());
        if (it != globals.functions.end()) {
          res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_CallNode, 0, "CALL " + *it, *it), call));
        }
      }

      res.push_back(std::make_pair(
        new CFG_Node(CFG_NodeType::CFG_AssignNode, 0, "=", new CFG_Opd(CFG_OpdType::CFG_UsevarOpd), new CFG_Opd(CFG_OpdType::CFG_VarOpd, it->second), nullptr),
        &inst
      )); // TODO: only do this if this has a use that we will not track
      /*res.push_back("USEVAR = " + it->second);*/
    } else {
      auto it = globals.stores.find(&inst);
      if (it != globals.stores.end()) {
        if (llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(&inst)) {
          RHS rhs = get_assignment_value(store->getValueOperand(), globals, erased_uses);
          res.push_back(std::make_pair(
            new CFG_Node(CFG_NodeType::CFG_AssignNode, 0, rhs.op, new CFG_Opd(CFG_OpdType::CFG_VarOpd, it->second), rhs.ropd1, rhs.ropd2),
            store
          ));
          /*res.push_back(it->second + " = " + get_assignment_value(store->getValueOperand(), globals));*/
        } else if (llvm::ReturnInst* ret = llvm::dyn_cast<llvm::ReturnInst>(&inst)) {
          if (ret->getNumOperands() == 1) {
            RHS rhs = get_assignment_value(ret->getOperand(0), globals, erased_uses);
            res.push_back(std::make_pair(
              new CFG_Node(CFG_NodeType::CFG_AssignNode, 0, rhs.op, new CFG_Opd(CFG_OpdType::CFG_VarOpd, it->second), rhs.ropd1, rhs.ropd2),
              store
            ));
            /*res.push_back(it->second + " = " + get_assignment_value(ret->getOperand(0), globals));*/
          }
        }
      } else if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(&inst)) {
        llvm::Function* func = call->getCalledFunction();
        if (func != nullptr) {
          auto it = globals.functions.find(func->getName().str());
          if (it != globals.functions.end()) {
            res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_CallNode, 0, "CALL " + *it, *it), call));
          }
        }
      }
    }
  }

  if (res.empty()) {
    res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_EmptyNode, 0, ""), nullptr));
  }

  std::vector<std::pair<CFG_Node*, llvm::Value*>> filtered_res;
  for (auto pair : res) {
    if (erased_uses.find(pair.second) == erased_uses.end()) {
      pair.first->set_node_id(node_num++);
      filtered_res.push_back(pair);
    } else {
      delete pair.first;
    }
  }

  return filtered_res;
}

void convert_to_proc_cfg(llvm::Function* func, std::map<int, llvm::Value*>* node_to_llvm, const GlobalInfo& globals) {
  std::vector<CFG_Node*> instructions;

  std::map<llvm::BasicBlock*, std::set<llvm::BasicBlock*>> cfg_transitions = construct_llvm_cfg(func);
  std::map<llvm::BasicBlock*, std::pair<int, int>> basic_blocks;

  std::string funcName = func->getName().str();
  Procedure* proc = new Procedure(funcName);
  for (auto pair : cfg_transitions) {
    std::vector<std::pair<CFG_Node*, llvm::Value*>> nodes = get_nodes_in_basic_block(funcName, pair.first, globals);
    for (size_t i = 0; i < nodes.size(); ++i) {
      CFG_Node* node = nodes[i].first;
      node->set_parent_proc(funcName);
      program->add_cfg_node(node);
      proc->add_cfg_node(node);
      (*node_to_llvm)[node_to_llvm->size() + 1] = nodes[i].second;
    }
    int start_node_num = node_num - nodes.size();
    basic_blocks[pair.first] = {start_node_num, node_num - 1};

    for (int i = start_node_num; i < node_num - 1; ++i) {
      CFG_Edge* edge = new CFG_Edge(i, i + 1);
      edge->get_src()->add_out_edge(edge);
      edge->get_dst()->add_in_edge(edge);
      program->add_cfg_edge(edge);
      proc->add_cfg_edge(edge);
    }
  }

  CFG_Node* end_node = new CFG_Node(CFG_NodeType::CFG_EndNode, node_num, "END " + funcName);
  end_node->set_parent_proc(funcName);
  program->add_cfg_node(end_node);
  proc->add_cfg_node(end_node);
  (*node_to_llvm)[node_to_llvm->size() + 1] = nullptr;

  for (auto pair : cfg_transitions) {
    int bb1_end = basic_blocks[pair.first].second;
    for (llvm::BasicBlock* outgoing : pair.second) {
      int bb2_start = basic_blocks[outgoing].first;
      CFG_Edge* edge = new CFG_Edge(bb1_end, bb2_start);
      edge->get_src()->add_out_edge(edge);
      edge->get_dst()->add_in_edge(edge);
      program->add_cfg_edge(edge);
      proc->add_cfg_edge(edge);
    }
    if (pair.second.empty()) {
      CFG_Edge* edge = new CFG_Edge(bb1_end, node_num); // Transition to END node
      edge->get_src()->add_out_edge(edge);
      edge->get_dst()->add_in_edge(edge);
      program->add_cfg_edge(edge);
      proc->add_cfg_edge(edge);
    }
  }
  ++node_num; // END node
  program->add_proc(proc);
  program->push_proc(proc);
}

std::map<int, llvm::Value*> llvm_parse() {
  std::map<int, llvm::Value*> node_to_llvm;
  GlobalInfo globals = get_globals(module);
  for (auto& func : *module) {
    if (!is_usable_func(&func)) {
      continue;
    }

    convert_to_proc_cfg(&func, &node_to_llvm, globals);
  }

  return node_to_llvm;
}

