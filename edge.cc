#include "edge.hh"

extern void cleanup(const char *);

Edge::Edge(int src_num, int dst_num) {
  this->src_num = src_num;
  this->dst_num = dst_num;
  this->src = NULL;
  this->dst = NULL;
}

Edge::~Edge() {}

void Edge::visualize() {
  cout << "Edge from " << this->src->node_num << " to " << this->dst->node_num
       << endl;
}
