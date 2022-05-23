#include "headers.hh"

using namespace std;

extern fstream *dot_fd;

Program::Program() {
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

Procedure *Program::get_proc(string name) {
  if (this->procedures->find(name) == this->procedures->end()) {
    CHECK_INPUT_AND_ABORT(false, "Procedure " + name + " not found.");
  }
  return this->procedures->find(name)->second;
}

CFG_Node *Program::get_cfg_node(int node_id) {
  if (this->cfg_nodes->find(node_id) == this->cfg_nodes->end()) {
    CHECK_INPUT_AND_ABORT(false,
                          "CFG node " + to_string(node_id) + " not found.");
  }
  return this->cfg_nodes->find(node_id)->second;
}

SSA_Node *Program::get_ssa_node(int node_id) {
  if (this->ssa_nodes->find(node_id) == this->ssa_nodes->end()) {
    CHECK_INPUT_AND_ABORT(false,
                          "SSA node " + to_string(node_id) + " not found.");
  }
  return this->ssa_nodes->find(node_id)->second;
}

void Program::visualize_cfg() {
  *dot_fd << "digraph G {\n";
  for (list<Procedure *>::iterator it = this->procs->begin();
       it != this->procs->end(); ++it) {
    (*it)->visualize_cfg();
  }
  *dot_fd << "}\n";
}
