#ifndef _CFG_EDGE_HH_
#define _CFG_EDGE_HH_

#include <utility>

class CFG_Node;

class CFG_Edge {
public:
  /* Always initialized: by constructor */
  std::pair<int, int> edge_id;
  CFG_Node *src;
  CFG_Node *dst;

  /* Constructors and Destructor */
  CFG_Edge(int src_id, int dst_id);
  ~CFG_Edge();

  /* Helper functions */
  void visualize();
};

#endif
