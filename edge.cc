#include "edge.hh"

extern fstream *gv_fd;
extern void cleanup(const char *);

Edge::Edge(int src_num, int dst_num) {
  this->src_num = src_num;
  this->dst_num = dst_num;
  this->src = NULL;
  this->dst = NULL;
}

Edge::~Edge() {}

void Edge::visualize() {
  *gv_fd << "\t\tnode_" << this->src_num << " -> "
          << "node_" << this->dst_num << ";" << endl;
}
