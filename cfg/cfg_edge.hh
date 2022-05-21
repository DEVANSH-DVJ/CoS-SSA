#ifndef _CFG_EDGE_HH_
#define _CFG_EDGE_HH_

class CFG_Node;

class CFG_Edge {
public:
  CFG_Node *src;
  CFG_Node *dst;

  CFG_Edge(CFG_Node *src, CFG_Node *dst);
  ~CFG_Edge();

  void visualize();
};

#endif
