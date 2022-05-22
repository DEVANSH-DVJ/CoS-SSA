#ifndef _CFG_NODE_HH_
#define _CFG_NODE_HH_

#include <string>

#include <map>

typedef enum {
  CFG_StartNode,
  CFG_EndNode,
  CFG_CallNode,
  CFG_InputNode,
  CFG_UsevarNode,
  CFG_ExprNode,
} CFG_NodeType;

class CFG_Edge;
class CFG_Opd;

class CFG_Node {
public:
  CFG_NodeType type;
  int node_id;

  std::string parent_proc;
  std::string stmt;
  std::map<int, CFG_Edge *> *in_edges;
  std::map<int, CFG_Edge *> *out_edges;

  std::string callee_proc;
  CFG_Opd *lopd;
  CFG_Opd *ropd1;
  CFG_Opd *ropd2;
  std::string op;

  CFG_Node(CFG_NodeType type, int node_id);
  ~CFG_Node();

  void visualize();
};

#endif
