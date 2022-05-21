#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;

CFG_Edge::CFG_Edge(CFG_Node *src, CFG_Node *dst) {
  this->src = src;
  this->dst = dst;
}

CFG_Edge::~CFG_Edge() {}

void CFG_Edge::visualize() {
  *dot_fd << "\tnode_" << this->src->node_id << " -> "
          << "node_" << this->dst->node_id << ";\n";
}
