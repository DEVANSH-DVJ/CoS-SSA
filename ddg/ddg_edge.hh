#ifndef _DDG_EDGE_HH_
#define _DDG_EDGE_HH_

#include <iostream>
#include <string>

using namespace std;

#include "../headers.hh"

#include "ddg_node.hh"

class DDG_Edge {
public:
  DDG_Node *src;
  DDG_Node *dst;

  // DDG_Edge(DDG_Node *src, DDG_Node *dst);
  // ~DDG_Edge();
};

#endif
