#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;

SSA_Node::SSA_Node(SSA_NodeType type, int node_id) {
  this->type = type;
  this->node_id = node_id;

  this->parent_proc = "";
  this->metas = new map<int, SSA_Meta *>();
  this->in_edges = new map<int, SSA_Edge *>();
  this->out_edges = new map<int, SSA_Edge *>();
}

SSA_Node::~SSA_Node() {
  for (map<int, SSA_Meta *>::iterator it = this->metas->begin();
       it != this->metas->end(); ++it) {
    delete it->second;
  }
  delete this->metas;
  delete this->in_edges;
  delete this->out_edges;
}

void SSA_Node::visualize() {
  *dot_fd << "\t\tnode_" << this->node_id << " [shape=record, xlabel=\""
          << this->node_id << "\", label=\"";
  for (map<int, SSA_Meta *>::iterator it = this->metas->begin();
       it != this->metas->end(); ++it) {
    it->second->visualize();
    if (it != --this->metas->end()) {
      *dot_fd << " | ";
    }
  }

  *dot_fd << "\"];\n";
}
