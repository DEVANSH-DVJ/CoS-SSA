#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"

#include <iostream>
#include <llvm/IR/CFG.h>
#include <memory>
#include <map>
#include <set>
#include <queue>
#include <fstream>

int node_num = 1;

std::string func_ret_to_global(llvm::Function* func) {
  return "$" + func->getName().str() + "_ret";
}

bool is_usable_func(llvm::Function* func) {
  // Get other generated code (like templates), use return !func->isDeclaration();
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

bool get_operand_repr(llvm::Value* operand, std::string* repr, const GlobalInfo& globals) {
  if (llvm::Instruction* inst = llvm::dyn_cast<llvm::Instruction>(operand)) {
    auto it = globals.loads.find(inst);
    if (it != globals.loads.end()) {
      *repr = it->second;
      return true;
    }
  } else if (llvm::ConstantInt* value = llvm::dyn_cast<llvm::ConstantInt>(operand)) {
    llvm::SmallVector<char> vec;
    value->getValue().toStringSigned(vec);
    *repr = std::string(vec.begin(), vec.end());
    return true;
  }

  return false;
}

std::string get_assignment_value(llvm::Value* value, const GlobalInfo& globals) {
  static const std::map<unsigned int, char> ops {
    {llvm::Instruction::Add, '+'},
    {llvm::Instruction::Sub, '-'},
    {llvm::Instruction::Mul, '*'},
    {llvm::Instruction::SDiv, '/'}
  };

  std::string s;
  if (get_operand_repr(value, &s, globals)) {
    return s;
  }
  if (llvm::Instruction* inst = llvm::dyn_cast<llvm::Instruction>(value)) {
    auto it = ops.find(inst->getOpcode());
    if (it != ops.end()) {
      if (inst->getNumOperands() == 2) {
        std::string s2;
        if (get_operand_repr(inst->getOperand(0), &s, globals) && get_operand_repr(inst->getOperand(1), &s2, globals)) {
          return s + ' ' + it->second + ' ' + s2;
        }
      }
    }
  }

  return "INPUT";
}

std::vector<std::string> get_instructions_in_basic_block(const std::string& proc, llvm::BasicBlock* bb, const GlobalInfo& globals) {
  std::vector<std::string> res;

  if (bb->isEntryBlock()) {
    res.push_back("START " + proc);

    if (proc == "main") {
      for (llvm::GlobalVariable* var : globals.globals) {
        std::string s;
        if (get_operand_repr(var->getOperand(0), &s, globals)) {
          res.push_back(var->getName().str() + " = " + s);
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
          res.push_back("CALL " + *it);
        }
      }

      res.push_back("USEVAR = " + it->second);
    } else {
      auto it = globals.stores.find(&inst);
      if (it != globals.stores.end()) {
        if (llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(&inst)) {
          res.push_back(it->second + " = " + get_assignment_value(store->getValueOperand(), globals));
        } else if (llvm::ReturnInst* ret = llvm::dyn_cast<llvm::ReturnInst>(&inst)) {
          if (ret->getNumOperands() == 1) {
            res.push_back(it->second + " = " + get_assignment_value(ret->getOperand(0), globals));
          }
        }
      } else if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(&inst)) {
        auto it = globals.functions.find(call->getCalledFunction()->getName().str());
        if (it != globals.functions.end()) {
          res.push_back("CALL " + *it);
        }
      }
    }
  }

  if (res.empty()) {
    res.push_back("null = 0");
  }

  return res;
}

typedef struct SimpleCFG {
  std::vector<std::string> instructions;
  std::map<int, std::set<int>> transitions;
  int node_num_start;
} SimpleCFG;

typedef struct SimpleBasicBlock {
  std::vector<std::string> instructions;
  int node_num_start;
} SimpleBasicBlock;

SimpleCFG convert_to_simple_cfg(llvm::Function* func,
                                const GlobalInfo& globals) {
  SimpleCFG cfg;
  cfg.node_num_start = node_num;

  std::map<llvm::BasicBlock*, std::set<llvm::BasicBlock*>> cfg_transitions = construct_llvm_cfg(func);
  std::map<llvm::BasicBlock*, SimpleBasicBlock> basic_blocks;
  int num_insts = 0;
  // Jank way of making sure we output the start node first
  for (int i = 0; i < 2; ++i) {
    for (auto pair : cfg_transitions) {
      if (pair.first->isEntryBlock() == (i == 0)) {
        SimpleBasicBlock bb {get_instructions_in_basic_block(func->getName().str(), pair.first, globals), node_num};
        basic_blocks[pair.first] = bb;
        num_insts += bb.instructions.size();
        node_num += bb.instructions.size();

        for (int i = bb.node_num_start; i < node_num - 1; ++i) {
          cfg.transitions[i].insert(i + 1);
        }
      }
    }
  }

  for (auto pair : cfg_transitions) {
    SimpleBasicBlock& bb1 = basic_blocks[pair.first];
    int bb1_end_node = bb1.node_num_start + bb1.instructions.size() - 1;
    for (llvm::BasicBlock* outgoing : pair.second) {
      SimpleBasicBlock& bb2 = basic_blocks[outgoing];

      cfg.transitions[bb1_end_node].insert(bb2.node_num_start);
    }
    if (pair.second.empty()) {
      cfg.transitions[bb1_end_node].insert(node_num); // Transition to END node
    }
  }
  ++node_num; // END node

  if (num_insts == 1) {
    cfg.transitions[cfg.node_num_start] = {cfg.node_num_start + 1};
    cfg.transitions[cfg.node_num_start + 1] = {cfg.node_num_start + 2};
    cfg.instructions.resize(3);
    cfg.instructions[1] = "null = 0";
    ++num_insts;
    ++node_num;
  } else {
    cfg.instructions.resize(num_insts + 1);
  }

  cfg.instructions[num_insts] = "END " + func->getName().str();
  for (auto pair : basic_blocks) {
    for (size_t i = 0; i < pair.second.instructions.size(); ++i) {
      cfg.instructions[pair.second.node_num_start + i - cfg.node_num_start] = pair.second.instructions[i];
    }
  }

  return cfg;
}

void output_cfg(const std::map<std::string, SimpleCFG>& procs, std::ofstream& output) {
  bool first = true;
  for (auto& pair : procs) {
    if (first) {
      first = false;
    } else {
      output << ", ";
    }
    output << pair.first;
  }
  output << ";\n";

  for (auto& pair : procs) {
    output << "\n{\n";
    int node = pair.second.node_num_start;
    for (const std::string& inst : pair.second.instructions) {
      output << "\t" << node << ": " << inst << ";\n";
      ++node;
    }
    output << '\n';
    for (auto transition : pair.second.transitions) {
      for (int dest : transition.second) {
        output << "\t" << transition.first << "->" << dest << ";\n";
      }
    }
    output << "}\n";
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <input file>.ll <output file>";
    return 1;
  }

  llvm::LLVMContext context;
  llvm::SMDiagnostic err;

  std::unique_ptr<llvm::Module> module = llvm::parseIRFile(argv[1], err, context);
  if (module == nullptr) {
    err.print(argv[0], llvm::errs());
    return 1;
  }

  std::ofstream output {argv[2]};
  if (!output.is_open()) {
    std::cerr << "Failed to open output file " << argv[2] << '\n';
    return 1;
  }

  std::map<std::string, SimpleCFG> procs;
  GlobalInfo globals = get_globals(module.get());
  for (auto& func : *module) {
    if (!is_usable_func(&func)) {
      continue;
    }

    SimpleCFG cfg = convert_to_simple_cfg(&func, globals);
    procs[func.getName().str()] = cfg;
  }

  output_cfg(procs, output);
}
