#include "../headers.hh"
#include "ddg_types.hh"

#include <map>
#include <queue>
#include <set>
#include <vector>

extern Program* program;

void ddg_construct() {
  int default_context = program->insert_ddg_context({"main"});

  int start_main = program->get_proc("main")->get_start_node();
  std::map<QNode, std::set<QDef>> rd_in;
  rd_in[{start_main, default_context}] = std::set<QDef>();
  for (const std::string& var_name : program->get_globals()) {
    rd_in[{start_main, default_context}].insert({var_name, 0, default_context});
    program->add_ddg_node({var_name, 0, default_context});
  }
  std::map<QNode, std::set<QDef>> rd_out;

  std::queue<QNode> worklist;
  worklist.push({start_main, default_context});
  std::set<QNode> in_list;
  in_list.insert({start_main, default_context});

  while (!worklist.empty()) {
    QNode cur_qnode = worklist.front();
    worklist.pop();
    in_list.erase(in_list.find(cur_qnode));

    std::set<QDef> orig_rd_out = rd_out[cur_qnode];

    CFG_Node* node = program->get_cfg_node(cur_qnode.node, true);
    if (node->get_type() == CFG_NodeType::CFG_StartNode) {
      auto it = program->get_ddg_reverse_transitions(cur_qnode.context);
      if (it != program->ddg_reverse_transitions_end()) {
        for (QNode qnode : it->second) {
          for (QDef qdef : rd_in[qnode]) {
            rd_in[cur_qnode].insert(qdef);
          }
        }
      }
    } else {
      for (int pred : node->get_predecessors()) {
        for (QDef qdef : rd_out[{pred, cur_qnode.context}]) {
          rd_in[cur_qnode].insert(qdef);
        }
      }
    }

    if (node->get_type() == CFG_NodeType::CFG_AssignNode) {
      const std::string& def = node->get_def();
      std::set<std::string> uses = node->get_uses();
      QDef new_qdef = {{def, cur_qnode.node}, cur_qnode.context};
      program->add_ddg_node(new_qdef);
      for (QDef qdef : rd_in[cur_qnode]) {
        if (uses.find(qdef.def.var_name) != uses.end()) {
          program->add_ddg_edge(qdef, new_qdef);
        }
      }
    }

    bool updated_transition = false;
    if (node->get_type() == CFG_NodeType::CFG_CallNode) {
      auto it = program->get_ddg_transition(cur_qnode);
      if (it != program->ddg_transitions_end()) {
        int end_node = program->get_proc(node->get_callee())->get_end_node();
        for (QDef qdef : rd_out[{end_node, it->second}]) {
          rd_out[cur_qnode].insert(qdef);
        }
      }

      updated_transition = program->create_ddg_transition(cur_qnode, Context::gen_context(node->get_callee(), rd_in[cur_qnode]));
    } else {
      rd_out[cur_qnode] = rd_in[cur_qnode];
      if (node->get_type() == CFG_NodeType::CFG_AssignNode) {
        // Applies rd_kill
        std::string killed_var = node->get_def();
        std::vector<QDef> to_remove;
        for (QDef qdef : rd_out[cur_qnode]) {
          if (qdef.def.var_name == killed_var) {
            to_remove.push_back(qdef);
          }
        }
        for (QDef qdef : to_remove) {
          rd_out[cur_qnode].erase(rd_out[cur_qnode].find(qdef));
        }

        // Applies rd_gen
        rd_out[cur_qnode].insert({{killed_var, cur_qnode.node}, cur_qnode.context});
      }
    }

    if (updated_transition) {
      // Update at start of call
      int start_node = program->get_proc(node->get_callee())->get_start_node();
      auto it = program->get_ddg_transition(cur_qnode);
      if (it != program->ddg_transitions_end()) {
        if (in_list.find({start_node, it->second}) == in_list.end()) {
          worklist.push({start_node, it->second});
          in_list.insert({start_node, it->second});
        }
      }
    }

    if (rd_out[cur_qnode] != orig_rd_out) {
      if (node->get_type() == CFG_NodeType::CFG_EndNode) {
        // Update at return from call
        auto it = program->get_ddg_reverse_transitions(cur_qnode.context);
        if (it != program->ddg_reverse_transitions_end()) {
          for (QNode qnode : it->second) {
            if (in_list.find(qnode) == in_list.end()) {
              worklist.push(qnode);
              in_list.insert(qnode);
            }
          }
        }
      } else {
        for (int succ : node->get_successors()) {
          if (in_list.find({succ, cur_qnode.context}) == in_list.end()) {
            worklist.push({succ, cur_qnode.context});
            in_list.insert({succ, cur_qnode.context});
          }
        }
      }
    }
  }
}

bool get_operand_value(CFG_Opd* opd, int* opd_value, QDef qdef, std::map<QDef, int>& propagated_values) {
  switch (opd->get_type()) {
    case CFG_OpdType::CFG_NumOpd: {
      *opd_value = opd->get_opd_value();
      return true;
    }
    case CFG_OpdType::CFG_VarOpd: {
      bool found = false;
      for (QDef dependency : program->get_ddg_incoming(qdef)) {
        if (dependency.def.var_name == opd->get_opd_var()) {
          auto it = propagated_values.find(dependency);
          if (it == propagated_values.end()) {
            return false;
          }
          if (found && *opd_value != it->second) {
            return false;
          }
          found = true;
          *opd_value = it->second;
        }
      }
      return found;
    }
    default: return false;
  }
}

bool propagate_value(QDef qdef, std::map<QDef, int>& propagated_values) {
  if (qdef.def.node == 0 || propagated_values.find(qdef) != propagated_values.end()) {
    return false;
  }

  CFG_Node* node = program->get_cfg_node(qdef.def.node, true);
  std::vector<CFG_Opd*> operands = node->get_rhs_operands();
  std::string op = node->get_op();
  if (op == "=") {
    CHECK_INVARIANT(operands.size() == 1, "Expected 1 operand");
    int value;
    if (get_operand_value(operands[0], &value, qdef, propagated_values)) {
      propagated_values[qdef] = value;
      return true;
    }
    return false;
  }

  CHECK_INVARIANT(operands.size() == 2, "Expected 2 operands");
  int v1, v2;
  if (get_operand_value(operands[0], &v1, qdef, propagated_values)
      && get_operand_value(operands[1], &v2, qdef, propagated_values)) {
    if (op == "+") {
      propagated_values[qdef] = v1 + v2;
      return true;
    } else if (op == "-") {
      propagated_values[qdef] = v1 - v2;
      return true;
    } else if (op == "*") {
      propagated_values[qdef] = v1 * v2;
      return true;
    } else if (op == "/") {
      propagated_values[qdef] = v1 / v2;
      return true;
    }
  }

  return false;
}

std::map<QDef, int> ddg_propagate_constants() {
  std::map<QDef, int> propagated_values; // Also serves as an "in_list"
  std::queue<QDef> worklist;
  for (QDef qdef : program->get_ddg_nodes()) {
    if (propagate_value(qdef, propagated_values)) {
      worklist.push(qdef);
    }
  }

  while (!worklist.empty()) {
    QDef qdef = worklist.front();
    worklist.pop();

    for (QDef outgoing : program->get_ddg_outgoing(qdef)) {
      if (propagate_value(outgoing, propagated_values)) {
        worklist.push(outgoing);
      }
    }
  }

  return propagated_values;
}
