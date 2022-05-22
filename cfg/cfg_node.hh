#ifndef _CFG_NODE_HH_
#define _CFG_NODE_HH_

#include <string>

#include <map>

class CFG_Edge;
class CFG_Opd;

typedef enum {
  CFG_StartNode,
  CFG_EndNode,
  CFG_CallNode,
  CFG_InputNode,
  CFG_UsevarNode,
  CFG_ExprNode,
} CFG_NodeType;

class CFG_Node {
public:
  /* Always initialized: by constructor */
  CFG_NodeType type;
  int node_id;

  /* Always initialized: post parsing*/
  std::string parent_proc;
  std::string stmt;
  std::map<int, CFG_Edge *> *in_edges;
  std::map<int, CFG_Edge *> *out_edges;

  /* Conditionally initialized: post parsing */
  std::string callee_proc;
  std::string op;
  CFG_Opd *lopd;
  CFG_Opd *ropd1;
  CFG_Opd *ropd2;

  /* Constructors and Destructor */
  CFG_Node(CFG_NodeType type, int node_id);
  ~CFG_Node();

  /* Helper functions */
  void visualize();
};

#endif
