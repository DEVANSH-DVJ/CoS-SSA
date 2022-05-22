#ifndef _SSA_NODE_HH_
#define _SSA_NODE_HH_

#include <map>

class SSA_Meta;

typedef enum {
  SSA_StartNode,
  SSA_EndNode,
  SSA_CallNode,
  SSA_InputNode,
  SSA_UsevarNode,
  SSA_ExprNode,
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
