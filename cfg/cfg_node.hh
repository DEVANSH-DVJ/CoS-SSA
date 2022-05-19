#ifndef _CFG_NODE_HH_
#define _CFG_NODE_HH_

#include <iostream>
#include <string>

using namespace std;

#include "../headers.hh"

typedef enum {
  CFG_Start,
  CFG_End,
  CFG_Call,
  CFG_Input,
  CFG_Usevar,
  CFG_Expr,
} CFG_NodeType;

class CFG_Node {
public:
  CFG_NodeType type;
  int node_id;

  // CFG_Node(CFG_NodeType type, int node_id);
  // ~CFG_Node();
};

#endif
