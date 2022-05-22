#ifndef _SSA_NODE_HH_
#define _SSA_NODE_HH_

#include <map>

class SSA_Meta;

typedef enum {
  SSA_Start,
  SSA_End,
  SSA_Call,
  SSA_Input,
  SSA_Usevar,
  SSA_Expr,
} SSA_NodeType;

class SSA_Node {
public:
  SSA_NodeType type;
  int node_id;

  map<int, SSA_Meta *> metas;

  // SSA_Node(SSA_NodeType type, int node_id);
  // ~SSA_Node();
};

#endif
