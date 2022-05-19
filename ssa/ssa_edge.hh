#ifndef _SSA_EDGE_HH_
#define _SSA_EDGE_HH_

#include <iostream>
#include <string>

using namespace std;

#include "../headers.hh"

#include "ssa_node.hh"

class SSA_Edge {
public:
  SSA_Node *src;
  SSA_Node *dst;

  // SSA_Edge(SSA_Node *src, SSA_Node *dst);
  // ~SSA_Edge();
};

#endif
