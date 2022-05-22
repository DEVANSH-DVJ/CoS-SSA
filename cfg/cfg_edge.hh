#ifndef _CFG_EDGE_HH_
#define _CFG_EDGE_HH_

#include <utility>

class CFG_Node;

class CFG_Edge {
public:
  std::pair<int, int> edge_id;
  CFG_Node *src;
  CFG_Node *dst;

  CFG_Edge(CFG_Node *src, CFG_Node *dst);
  ~CFG_Edge();

  void visualize();
};

#endif
