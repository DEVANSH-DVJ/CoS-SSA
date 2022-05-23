#include "headers.hh"

using namespace std;

extern fstream *dot_fd;

Procedure::Procedure(string name) {
  this->name = name;
  this->cfg_nodes = new map<int, CFG_Node *>();
  this->cfg_edges = new map<pair<int, int>, CFG_Edge *>();
  this->ssa_nodes = new map<int, SSA_Node *>();
  this->ssa_edges = new map<pair<int, int>, SSA_Edge *>();
}

Procedure::~Procedure() {
  delete this->cfg_nodes;
  delete this->cfg_edges;
  delete this->ssa_nodes;
  delete this->ssa_edges;
}

void Procedure::visualize_cfg() {
  *dot_fd << "\n\tsubgraph cluster_" << this->name << " {\n";
  *dot_fd << "\t\tlabel = \"" << this->name << "\";\n";
  *dot_fd << "\t\tmargin = 25;\n";
  for (map<int, CFG_Node *>::iterator it = this->cfg_nodes->begin();
       it != this->cfg_nodes->end(); ++it) {
    it->second->visualize();
  }
  for (map<pair<int, int>, CFG_Edge *>::iterator it = this->cfg_edges->begin();
       it != this->cfg_edges->end(); ++it) {
    it->second->visualize();
  }
  *dot_fd << "\t}\n";
}

void Procedure::visualize_ssa() {
  *dot_fd << "\n\tsubgraph cluster_" << this->name << " {\n";
  *dot_fd << "\t\tlabel = \"" << this->name << "\";\n";
  *dot_fd << "\t\tmargin = 25;\n";
  for (map<int, SSA_Node *>::iterator it = this->ssa_nodes->begin();
       it != this->ssa_nodes->end(); ++it) {
    it->second->visualize();
  }
  for (map<pair<int, int>, SSA_Edge *>::iterator it = this->ssa_edges->begin();
       it != this->ssa_edges->end(); ++it) {
    it->second->visualize();
  }
  *dot_fd << "\t}\n";
}
