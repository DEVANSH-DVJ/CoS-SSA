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
  /* Always initialized: by constructor */
  SSA_NodeType type;
  int node_id;

  /* Always initialized: post parsing*/
  std::string parent_proc;
  std::map<int, SSA_Meta *> *metas;

  /* Constructors and Destructor */
  SSA_Node(SSA_NodeType type, int node_id);
  ~SSA_Node();

  /* Helper functions */
  void visualize();
};

#endif
