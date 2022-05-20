#ifndef _CFG_EDGE_HH_
#define _CFG_EDGE_HH_

#include "cfg_node.hh"

class CFG_Edge {
public:
  CFG_Node *src;
  CFG_Node *dst;

  // CFG_Edge(CFG_Node *src, CFG_Node *dst);
  // ~CFG_Edge();
};

#endif
