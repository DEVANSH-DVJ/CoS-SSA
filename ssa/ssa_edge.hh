#ifndef _SSA_EDGE_HH_
#define _SSA_EDGE_HH_

#include "ssa_node.hh"

class SSA_Edge {
public:
  SSA_Node *src;
  SSA_Node *dst;

  // SSA_Edge(SSA_Node *src, SSA_Node *dst);
  // ~SSA_Edge();
};

#endif
