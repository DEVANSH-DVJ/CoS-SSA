#include "headers.hh"

using namespace std;

extern fstream *dot_fd;

Procedure::Procedure(string name) {
  this->name = name;
  this->cfg_nodes = new map<int, CFG_Node *>();
  this->cfg_edges = new map<pair<int, int>, CFG_Edge *>();
}

Procedure::~Procedure() {}

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
