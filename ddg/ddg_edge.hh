#ifndef _DDG_EDGE_HH_
#define _DDG_EDGE_HH_

#include "ddg_node.hh"

class DDG_Edge {
public:
  DDG_Node *src;
  DDG_Node *dst;

  // DDG_Edge(DDG_Node *src, DDG_Node *dst);
  // ~DDG_Edge();
};

#endif
