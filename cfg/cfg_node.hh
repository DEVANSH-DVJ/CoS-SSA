#ifndef _CFG_NODE_HH_
#define _CFG_NODE_HH_

#include <map>

typedef enum {
  CFG_Start,
  CFG_End,
  CFG_Call,
  CFG_Input,
  CFG_Usevar,
  CFG_Expr,
} CFG_NodeType;

class CFG_Edge;

class CFG_Node {
public:
  CFG_NodeType type;
  int node_id;

  std::map<int, CFG_Edge *> *in_edges;
  std::map<int, CFG_Edge *> *out_edges;

  CFG_Node(CFG_NodeType type, int node_id);
  ~CFG_Node();
};

#endif
