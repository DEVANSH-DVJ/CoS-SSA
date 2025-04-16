#include "program.hh"
#include "headers.hh"

#include <algorithm>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/Module.h>

using namespace std;

#include "cfg/cfg.tab.hh"
#include "ssa/ssa.tab.hh"

extern void cfg_set_in(FILE *);
extern void cfg_set_out(FILE *);

extern void ssa_set_in(FILE *);
extern void ssa_set_out(FILE *);

extern fstream *dot_fd;

Program::Program(string tool, string input_name) {
  this->tool = tool;
  this->input_name = input_name;

  if (this->tool == "cfg" || this->tool == "ddg") {
    string cfg_file = this->input_name + ".cfg";
    cfg_set_in(fopen(cfg_file.c_str(), "r"));
    cfg_set_out(fopen("/dev/null", "w"));
  } else if (this->tool == "ssa") {
    string ssa_file = this->input_name + ".ssa";
    ssa_set_in(fopen(ssa_file.c_str(), "r"));
    ssa_set_out(fopen("/dev/null", "w"));
  } else if (this->tool == "llvm" || this->tool == "all") {
    string ll_file = this->input_name + ".ll";
    string output_ll_file = this->input_name + ".out.ll";
    llvm_module = llvm::parseIRFile(ll_file, err, context);
    llvm_set_in(llvm_module.get());
    llvm_set_out(output_ll_file);
  } else {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Unknown input type");
  }

  this->procs = new list<Procedure *>();
  this->procedures = new map<string, Procedure *>();
  this->cfg_nodes = new map<int, CFG_Node *>();
  this->cfg_edges = new map<pair<int, int>, CFG_Edge *>();
  this->ssa_nodes = new map<int, SSA_Node *>();
  this->ssa_edges = new map<pair<int, int>, SSA_Edge *>();
}

Program::~Program() {
  for (map<string, Procedure *>::iterator it = this->procedures->begin();
       it != this->procedures->end(); ++it) {
    delete it->second;
  }
  delete this->procedures;

  for (map<int, CFG_Node *>::iterator it = this->cfg_nodes->begin();
       it != this->cfg_nodes->end(); ++it) {
    delete it->second;
  }
  delete this->cfg_nodes;

  for (map<pair<int, int>, CFG_Edge *>::iterator it = this->cfg_edges->begin();
       it != this->cfg_edges->end(); ++it) {
    delete it->second;
  }
  delete this->cfg_edges;

  for (map<int, SSA_Node *>::iterator it = this->ssa_nodes->begin();
       it != this->ssa_nodes->end(); ++it) {
    delete it->second;
  }
  delete this->ssa_nodes;

  for (map<pair<int, int>, SSA_Edge *>::iterator it = this->ssa_edges->begin();
       it != this->ssa_edges->end(); ++it) {
    delete it->second;
  }
  delete this->ssa_edges;
}

map<string, Procedure *> *Program::get_procs() { return this->procedures; }

Procedure *Program::get_proc(string name) {
  if (this->procedures->find(name) == this->procedures->end()) {
    CHECK_INPUT_AND_ABORT(false, "Procedure " + name + " not found.");
  }
  return this->procedures->find(name)->second;
}

CFG_Node *Program::get_cfg_node(int node_id, bool abort_if_not_found) {
  if (this->cfg_nodes->find(node_id) == this->cfg_nodes->end()) {
    CHECK_INPUT_AND_ABORT(!abort_if_not_found,
                          "CFG node " + to_string(node_id) + " not found.");
    CHECK_INPUT_AND_ABORT(true, "");
    CHECK_INPUT_AND_ABORT(false, "");
    return NULL;
  }
  return this->cfg_nodes->find(node_id)->second;
}

SSA_Node *Program::get_ssa_node(int node_id, bool abort_if_not_found) {
  if (this->ssa_nodes->find(node_id) == this->ssa_nodes->end()) {
    CHECK_INPUT_AND_ABORT(!abort_if_not_found,
                          "SSA node " + to_string(node_id) + " not found.");
    return NULL;
  }
  return this->ssa_nodes->find(node_id)->second;
}

llvm::Value* Program::get_llvm_node(int node_id, bool abort_if_not_found) {
  auto it = llvm_nodes.find(node_id);
  if (it == llvm_nodes.end()) {
    CHECK_INPUT_AND_ABORT(!abort_if_not_found,
                          "LLVM node " + to_string(node_id) + " not found.");
    return NULL;
  }
  return it->second;
}

void Program::add_proc(Procedure *proc) {
  CHECK_INVARIANT(proc != NULL, "Procedure cannot be NULL.");
  string name = proc->get_name();
  CHECK_INVARIANT(this->procedures->find(name) == this->procedures->end(),
                  "Procedure " + name + " already exists.");
  this->procedures->insert(make_pair(name, proc));
}

void Program::push_proc(Procedure *proc) {
  CHECK_INVARIANT(proc != NULL, "Procedure cannot be NULL.");
  string name = proc->get_name();
  for (list<Procedure *>::iterator it = this->procs->begin();
       it != this->procs->end(); ++it) {
    CHECK_INVARIANT((*it)->get_name() != name,
                    "Procedure " + name + " already exists.");
  }
  this->procs->push_back(proc);
}

void Program::add_cfg_node(CFG_Node *node) {
  CHECK_INVARIANT(node != NULL, "CFG node cannot be NULL.");
  int node_id = node->get_node_id();
  CHECK_INPUT_AND_ABORT(this->cfg_nodes->find(node_id) ==
                            this->cfg_nodes->end(),
                        "CFG node " + to_string(node_id) + " already exists.");
  this->cfg_nodes->insert(make_pair(node_id, node));
}

void Program::add_cfg_edge(CFG_Edge *edge) {
  CHECK_INVARIANT(edge != NULL, "CFG edge cannot be NULL.");
  pair<int, int> edge_id = edge->get_edge_id();
  CHECK_INPUT_AND_ABORT(this->cfg_edges->find(edge_id) ==
                            this->cfg_edges->end(),
                        "CFG edge (" + to_string(edge_id.first) + ", " +
                            to_string(edge_id.second) + ") already exists.");
  this->cfg_edges->insert(make_pair(edge_id, edge));
}

void Program::add_ssa_node(SSA_Node *node) {
  CHECK_INVARIANT(node != NULL, "SSA node cannot be NULL.");
  int node_id = node->get_node_id();
  CHECK_INPUT_AND_ABORT(this->ssa_nodes->find(node_id) ==
                            this->ssa_nodes->end(),
                        "SSA node " + to_string(node_id) + " already exists.");
  this->ssa_nodes->insert(make_pair(node_id, node));
}

void Program::add_ssa_edge(SSA_Edge *edge) {
  CHECK_INVARIANT(edge != NULL, "SSA edge cannot be NULL.");
  pair<int, int> edge_id = edge->get_edge_id();
  CHECK_INPUT_AND_ABORT(this->ssa_edges->find(edge_id) ==
                            this->ssa_edges->end(),
                        "SSA edge (" + to_string(edge_id.first) + ", " +
                            to_string(edge_id.second) + ") already exists.");
  this->ssa_edges->insert(make_pair(edge_id, edge));
}

void Program::parse_cfg_from_llvm() { llvm_nodes = llvm_parse(); }

void Program::parse_cfg() { cfg_parse(); }

void Program::parse_ssa() { ssa_parse(); }

void Program::construct_ddg() { ddg_construct(); }

void Program::propagate_ddg_constants() { ddg_propagated_values = ddg_propagate_constants(); }

void Program::construct_ssa() {
  ssa_construct();
}

void Program::deconstruct_ssa() {
  ssa_deconstruct();
}

void Program::dump_cfg() {
  string cfg_file = input_name + ".cfg";

  CHECK_INVARIANT(dot_fd == NULL, "Dot file descriptor must be NULL.");
  dot_fd = new fstream(cfg_file.c_str(), ios::out | ios::trunc);

  bool first = true;
  for (Procedure* proc : *procs) {
    if (first) {
      first = false;
    } else {
      *dot_fd << ", ";
    }

    *dot_fd << proc->get_name();
  }
  *dot_fd << ";";

  for (auto pair : *procedures) {
    pair.second->dump_cfg();
  }

  dot_fd->close();
  delete dot_fd;
}

void Program::visualize_cfg() {
  string dot_file = input_name + ".cfg.dot";
  string png_file = input_name + ".cfg.png";

  CHECK_INVARIANT(dot_fd == NULL, "Dot file descriptor must be NULL.");
  dot_fd = new fstream(dot_file.c_str(), ios::out | ios::trunc);

  *dot_fd << "digraph G {\n";
  for (list<Procedure *>::iterator it = this->procs->begin();
       it != this->procs->end(); ++it) {
    (*it)->visualize_cfg();
  }
  *dot_fd << "}\n";

  dot_fd->close();
  delete dot_fd;

  if (system(("dot -Tpng " + dot_file + " -o " + png_file).c_str()) != 0) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Error generating png file\n");
  }
}

void print_qdef(QDef node, const std::map<QDef, int>& propagated_values) {
  std::cout << node.def.var_name + '_' + std::to_string(node.def.node) + '_' + std::to_string(node.context);
  auto it = propagated_values.find(node);
  if (it != propagated_values.end()) {
    std::cout << " = " << it->second;
  }
}

void Program::visualize_ddg() {
  std::cout << ddg_context_table.to_string() << '\n';

  for (auto pair : ddg_context_transitions) {
    for (auto subpair : pair.second) {
      std::cout << "Context transition at node " << pair.first << ": " << subpair.first << " -> " << subpair.second << '\n';
    }
  }
  std::cout << '\n';

  std::vector<QDef> nodes (ddg_nodes.begin(), ddg_nodes.end());
  std::sort(nodes.begin(), nodes.end(), [](QDef l, QDef r) {
    return l.def.node < r.def.node;
  });

  for (QDef node : nodes) {
    print_qdef(node, ddg_propagated_values);
    std::cout << " <- ";
    bool first = true;
    for (QDef incoming : ddg_reverse_edges[node]) {
      if (first) {
        first = false;
      } else {
        std::cout << ", ";
      }
      print_qdef(incoming, ddg_propagated_values);
    }
    std::cout << '\n';
  }
}

void Program::dump_ssa() {
  string ssa_file = input_name + ".out.ssa";

  CHECK_INVARIANT(dot_fd == NULL, "Dot file descriptor must be NULL.");
  dot_fd = new fstream(ssa_file.c_str(), ios::out | ios::trunc);

  bool first = true;
  for (Procedure* proc : *procs) {
    if (first) {
      first = false;
    } else {
      *dot_fd << ", ";
    }

    *dot_fd << proc->get_name();
  }
  *dot_fd << ";";

  for (auto pair : *procedures) {
    pair.second->dump_ssa();
  }

  dot_fd->close();
  delete dot_fd;
}

void Program::visualize_ssa() {
  string dot_file = input_name + ".ssa.dot";
  string png_file = input_name + ".ssa.png";

  CHECK_INVARIANT(dot_fd == NULL, "Dot file descriptor must be NULL.");
  dot_fd = new fstream(dot_file.c_str(), ios::out | ios::trunc);

  *dot_fd << "digraph G {\n";
  for (list<Procedure *>::iterator it = this->procs->begin();
       it != this->procs->end(); ++it) {
    (*it)->visualize_ssa();
  }
  *dot_fd << "}\n";

  dot_fd->close();
  delete dot_fd;

  if (system(("dot -Tpng " + dot_file + " -o " + png_file).c_str()) != 0) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Error generating png file\n");
  }
}

void Program::dump_llvm() {
  llvm_dump();
}

std::set<std::string> Program::get_globals() {
  std::set<std::string> res;
  for (Procedure* proc : *procs) {
    for (std::string global : proc->get_globals()) {
      res.insert(global);
    }
  }
  return res;
}

std::set<QDef> Program::get_ddg_nodes() {
  return ddg_nodes;
}

std::set<QDef> Program::get_ddg_incoming(QDef node) {
  return ddg_reverse_edges[node];
}

std::set<QDef> Program::get_ddg_outgoing(QDef node) {
  return ddg_edges[node];
}

bool Program::create_ddg_transition(QNode from_qnode, const Context& to_context) {
  auto it = ddg_context_transitions[from_qnode.node].find(from_qnode.context);
  if (it != ddg_context_transitions[from_qnode.node].end()) {
    return ddg_context_table.update_context(it->second, to_context);
  }

  int context = ddg_context_table.insert_context(to_context);
  ddg_context_transitions[from_qnode.node][from_qnode.context] = context;
  ddg_reverse_context_transitions[context].insert(from_qnode);
  return true;
}

std::map<int, int>::iterator Program::get_ddg_transition(QNode from_qnode) {
  return ddg_context_transitions[from_qnode.node].find(from_qnode.context);
}

std::map<int, int>& Program::get_ddg_transitions(int node) {
  return ddg_context_transitions[node];
}

std::map<int, int>::iterator Program::ddg_transitions_end(int node) {
  return ddg_context_transitions[node].end();
}

std::map<int, std::set<QNode>>::iterator Program::get_ddg_reverse_transitions(int to_context) {
  return ddg_reverse_context_transitions.find(to_context);
}

  std::map<int, std::set<QNode>>::iterator Program::ddg_reverse_transitions_end() {
  return ddg_reverse_context_transitions.end();
}

bool Program::get_ddg_propagated_value(QDef qdef, int* value) {
  auto it = ddg_propagated_values.find(qdef);
  if (it != ddg_propagated_values.end()) {
    *value = it->second;
    return true;
  }
  return false;
}

int Program::insert_ddg_context(Context context) {
  return ddg_context_table.insert_context(context);
}

void Program::add_ddg_node(QDef node) {
  ddg_nodes.insert(node);
}

void Program::add_ddg_edge(QDef src, QDef dest) {
  ddg_nodes.insert(src);
  ddg_nodes.insert(dest);
  ddg_edges[src].insert(dest);
  ddg_reverse_edges[dest].insert(src);
}

void Program::cleanup() {}

void Program::run() {
  if (this->tool == "cfg") {
    this->parse_cfg();
    this->visualize_cfg();
  } else if (this->tool == "ssa") {
    this->parse_ssa();
    this->visualize_ssa();
  } else if (this->tool == "ddg") {
    this->parse_cfg();
    this->construct_ddg();
    this->propagate_ddg_constants();
    this->visualize_ddg();
  } else if (this->tool == "llvm") {
    this->parse_cfg_from_llvm();
    this->dump_cfg();
  } else if (this->tool == "all") {
    this->parse_cfg_from_llvm();
    this->construct_ddg();
    /*this->propagate_ddg_constants();*/
    this->construct_ssa();
    this->deconstruct_ssa();
    this->dump_llvm();
  } else {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Unknown input type");
  }
}
