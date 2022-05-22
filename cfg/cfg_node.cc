#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;

CFG_Node::CFG_Node(CFG_NodeType type, int node_id) {
  this->type = type;
  this->node_id = node_id;

  this->parent_proc = "";
  this->stmt = "";
  this->in_edges = new std::map<int, CFG_Edge *>();
  this->out_edges = new std::map<int, CFG_Edge *>();

  this->callee_proc = "";
  this->lopd = NULL;
  this->ropd1 = NULL;
  this->ropd2 = NULL;
  this->op = "";
}

CFG_Node::~CFG_Node() {
  delete this->in_edges;
  delete this->out_edges;
  if (this->lopd != NULL) {
    delete this->lopd;
  }
  if (this->ropd1 != NULL) {
    delete this->ropd1;
  }
  if (this->ropd2 != NULL) {
    delete this->ropd2;
  }
}

void CFG_Node::visualize() {
  *dot_fd << "\t\tnode_" << this->node_id << " [shape=box, xlabel=\""
          << this->node_id << "\", label=\"" << this->stmt << "\"];\n";
}
