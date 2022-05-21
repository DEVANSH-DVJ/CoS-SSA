#include "headers.hh"

using namespace std;

extern fstream *dot_fd;

Program::Program() {
  this->procedures = new map<string, Procedure *>();
  this->cfg_nodes = new map<int, CFG_Node *>();
  this->cfg_edges = new map<pair<int, int>, CFG_Edge *>();
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

void Program::visualize_cfg() {
  *dot_fd << "digraph G {\n";
  for (map<int, CFG_Node *>::iterator it = this->cfg_nodes->begin();
       it != this->cfg_nodes->end(); ++it) {
    it->second->visualize();
  }
  for (map<pair<int, int>, CFG_Edge *>::iterator it = this->cfg_edges->begin();
       it != this->cfg_edges->end(); ++it) {
    it->second->visualize();
  }
  *dot_fd << "}\n";
}
