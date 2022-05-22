#ifndef _SSA_EDGE_HH_
#define _SSA_EDGE_HH_

class SSA_Node;

class SSA_Edge {
public:
  SSA_Node *src;
  SSA_Node *dst;

  // SSA_Edge(SSA_Node *src, SSA_Node *dst);
  // ~SSA_Edge();
};

#endif
