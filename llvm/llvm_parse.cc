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

std::unique_ptr<llvm::Module> module = nullptr;

void llvm_set_in(std::string file) {
  ll_file = file;
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
  for (llvm::Function& func : *module) {
    for (llvm::BasicBlock& bb : func) {
      for (llvm::Instruction& inst : bb) {
        // Can ignore GEP because then this is not an int
        // TODO: some globals may have their addr taken by other globals
        if (llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(&inst)) {
          if (llvm::GlobalVariable* var = llvm::dyn_cast<llvm::GlobalVariable>(store->getValueOperand())) {
            addr_taken.insert(var->getName().str());
          }
        } else if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(&inst)) {
          for (unsigned int i = 0; i < call->getNumOperands() - 1; ++i) {
            if (llvm::GlobalVariable* var = llvm::dyn_cast<llvm::GlobalVariable>(call->getOperand(i))) {
              addr_taken.insert(var->getName().str());
            }
          }
        }
      }
    }
  }

  // Probably a better way to check if a global's addr is taken
  /*for (llvm::GlobalVariable& var : module->globals()) {*/
  /*  if (var.getNumOperands() != 1) {*/
  /*    addr_taken.insert(var.getName());*/
  /*    break;*/
  /*  }*/
  /*  bool found = false;*/
  /*  for (auto& use : var.uses()) {*/
  /*    llvm::User* user = use.getUser();*/
  /*    if (llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(user)) {*/
  /*      if (&var != load->getPointerOperand()) {*/
  /*        found = true;*/
  /*        break;*/
  /*      }*/
  /*    } else if (llvm::StoreInst* load = llvm::dyn_cast<llvm::StoreInst>(user)) {*/
  /*      if (&var != load->getPointerOperand()) {*/
  /*        found = true;*/
  /*        break;*/
  /*      }*/
  /*    } else {*/
  /*      found = true;*/
  /*      break;*/
  /*    }*/
  /*  }*/
  /*  if (found) {*/
  /*    addr_taken.insert(var.getName());*/
  /*    break;*/
  /*  }*/
  /*}*/

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
        } else if (llvm::GetElementPtrInst* gep = llvm::dyn_cast<llvm::GetElementPtrInst>(&inst)) {
          if (llvm::GlobalVariable* var = llvm::dyn_cast<llvm::GlobalVariable>(gep->getPointerOperand())) {
            if (addr_taken.find(var->getName().str()) == addr_taken.end()) {
              globals.globals.insert(var);
              globals.loads[&inst] = var->getName();
            }
          }
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

struct RHS {
  std::string op;
  CFG_Opd* ropd1;
  CFG_Opd* ropd2;
};

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
    *repr = new CFG_Opd(CFG_OpdType::CFG_NumOpd, std::string(vec.begin(), vec.end()));
    return true;
  }

  return false;
}

RHS get_assignment_value(llvm::Value* value, const GlobalInfo& globals) {
  static const std::map<unsigned int, char> ops {
    {llvm::Instruction::Add, '+'},
    {llvm::Instruction::Sub, '-'},
    {llvm::Instruction::Mul, '*'},
    {llvm::Instruction::SDiv, '/'}
  };

  CFG_Opd* ropd1;
  if (get_operand_repr(value, &ropd1, globals)) {
    return {"=", ropd1, nullptr};
  }
  if (llvm::Instruction* inst = llvm::dyn_cast<llvm::Instruction>(value)) {
    auto it = ops.find(inst->getOpcode());
    if (it != ops.end()) {
      if (inst->getNumOperands() == 2) {
        if (get_operand_repr(inst->getOperand(0), &ropd1, globals)) {
          CFG_Opd* ropd2;
          if (get_operand_repr(inst->getOperand(1), &ropd2, globals)) {
            return {std::to_string(it->second), ropd1, ropd2};
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

  if (bb->isEntryBlock()) {
    res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_StartNode, node_num++, "START " + proc), nullptr));

    if (proc == "main") {
      for (llvm::GlobalVariable* var : globals.globals) {
        CFG_Opd* ropd1;
        if (get_operand_repr(var->getOperand(0), &ropd1, globals)) {
          res.push_back(std::make_pair(
            new CFG_Node(CFG_NodeType::CFG_AssignNode, node_num++, "=", new CFG_Opd(CFG_OpdType::CFG_VarOpd, var->getName().str()), ropd1, nullptr),
            var));
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
          res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_CallNode, node_num++, "CALL " + *it, *it), call));
        }
      }

      res.push_back(std::make_pair(
        new CFG_Node(CFG_NodeType::CFG_AssignNode, node_num++, "=", new CFG_Opd(CFG_OpdType::CFG_UsevarOpd), new CFG_Opd(CFG_OpdType::CFG_VarOpd, it->second), nullptr),
        &inst
      )); // TODO: is this necessary?
      /*res.push_back("USEVAR = " + it->second);*/
    } else {
      auto it = globals.stores.find(&inst);
      if (it != globals.stores.end()) {
        if (llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(&inst)) {
          RHS rhs = get_assignment_value(store->getValueOperand(), globals);
          res.push_back(std::make_pair(
            new CFG_Node(CFG_NodeType::CFG_AssignNode, node_num++, rhs.op, new CFG_Opd(CFG_OpdType::CFG_VarOpd, it->second), rhs.ropd1, rhs.ropd2),
            store
          ));
          /*res.push_back(it->second + " = " + get_assignment_value(store->getValueOperand(), globals));*/
        } else if (llvm::ReturnInst* ret = llvm::dyn_cast<llvm::ReturnInst>(&inst)) {
          if (ret->getNumOperands() == 1) {
            RHS rhs = get_assignment_value(ret->getOperand(0), globals);
            res.push_back(std::make_pair(
              new CFG_Node(CFG_NodeType::CFG_AssignNode, node_num++, rhs.op, new CFG_Opd(CFG_OpdType::CFG_VarOpd, it->second), rhs.ropd1, rhs.ropd2),
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
            res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_CallNode, node_num++, "CALL " + *it, *it), call));
          }
        }
      }
    }
  }

  if (res.empty()) {
    res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_EmptyNode, node_num++, ""), nullptr));
  }

  for (auto pair : res) {
    pair.first->set_parent_proc(proc);
  }

  return res;
}

void convert_to_proc_cfg(llvm::Function* func, std::vector<llvm::Value*>* node_to_llvm, const GlobalInfo& globals) {
  std::vector<CFG_Node*> instructions;

  std::map<llvm::BasicBlock*, std::set<llvm::BasicBlock*>> cfg_transitions = construct_llvm_cfg(func);
  std::map<llvm::BasicBlock*, std::pair<int, int>> basic_blocks;

  Procedure* proc = new Procedure(func->getName().str());
  for (auto pair : cfg_transitions) {
    std::vector<std::pair<CFG_Node*, llvm::Value*>> nodes = get_nodes_in_basic_block(func->getName().str(), pair.first, globals);
    for (size_t i = 0; i < nodes.size(); ++i) {
      CFG_Node* node = nodes[i].first;
      program->add_cfg_node(node);
      proc->add_cfg_node(node);
      node_to_llvm->push_back(nodes[i].second);
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

  CFG_Node* end_node = new CFG_Node(CFG_NodeType::CFG_EndNode, node_num, "END " + func->getName().str());
  end_node->set_parent_proc(func->getName().str());
  program->add_cfg_node(end_node);
  proc->add_cfg_node(end_node);

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

std::vector<llvm::Value*> llvm_parse() {
  llvm::LLVMContext context;
  llvm::SMDiagnostic err;

  module = llvm::parseIRFile(ll_file, err, context);
  if (module == nullptr) {
    err.print("Failed to parse LLVM IR file: ", llvm::errs());
    CHECK_INVARIANT(false, "");
  }

  std::vector<llvm::Value*> node_to_llvm {nullptr};
  GlobalInfo globals = get_globals(module.get());
  for (auto& func : *module) {
    if (!is_usable_func(&func)) {
      continue;
    }

    convert_to_proc_cfg(&func, &node_to_llvm, globals);
  }

  return node_to_llvm;
}

