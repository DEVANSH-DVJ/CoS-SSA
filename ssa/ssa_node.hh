#ifndef _SSA_NODE_HH_
#define _SSA_NODE_HH_

#include <map>

class SSA_Edge;
class SSA_Meta;

typedef enum {
  SSA_StartNode,
  SSA_EndNode,
  SSA_CallNode,
  SSA_AssignNode,
} SSA_NodeType;

class SSA_Node {
public:
  /* Always initialized: by constructor */
  SSA_NodeType type;
  int node_id;

  /* Always initialized: post parsing*/
  std::string parent_proc;
  std::map<int, SSA_Edge *> *in_edges;
  std::map<int, SSA_Edge *> *out_edges;

  /* Conditionally initialized: post parsing */
  std::string stmt;
  std::string callee_proc;
  std::map<int, SSA_Meta *> *metas;

  /* Constructors and Destructor */
  SSA_Node(SSA_NodeType type, int node_id, std::string stmt);
  SSA_Node(SSA_NodeType type, int node_id, std::string stmt,
           std::string callee_proc);
  SSA_Node(SSA_NodeType type, int node_id);
  ~SSA_Node();

  /* Helper functions */
  void visualize();
};

#endif
