#include "../headers.hh"

using namespace std;

CFG_Edge::CFG_Edge(CFG_Node *src, CFG_Node *dst) {
  this->src = src;
  this->dst = dst;
}

CFG_Edge::~CFG_Edge() {}
