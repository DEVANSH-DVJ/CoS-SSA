#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;

SSA_Node::SSA_Node(SSA_NodeType type, int node_id) {
  this->type = type;
  this->node_id = node_id;

  this->parent_proc = "";
  this->in_edges = new map<int, SSA_Edge *>();
  this->out_edges = new map<int, SSA_Edge *>();

  this->stmt = "";
  this->callee_proc = "";
  this->metas = new map<int, SSA_Meta *>();
}

SSA_Node::~SSA_Node() {
  for (map<int, SSA_Meta *>::iterator it = this->metas->begin();
       it != this->metas->end(); ++it) {
    delete it->second;
  }

  delete this->in_edges;
  delete this->out_edges;
  delete this->metas;
}

void SSA_Node::visualize() {
  if (this->type != SSA_AssignNode) {
    *dot_fd << "\t\tnode_" << this->node_id << " [shape=box, xlabel=\""
            << this->node_id << "\", label=\"" << this->stmt << "\"];\n";
  } else {
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
}
