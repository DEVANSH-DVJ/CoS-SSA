#ifndef _CFG_EDGE_HH_
#define _CFG_EDGE_HH_

#include <utility>

class CFG_Node;

class CFG_Edge {
  /* Always initialized: by constructor */
  std::pair<int, int> edge_id;
  CFG_Node *src;
  CFG_Node *dst;

public:
  /* Constructors and Destructor */
  CFG_Edge(int src_id, int dst_id);
  ~CFG_Edge();

  /* Helper functions */
  void visualize(); // Visualize the edge
  const std::pair<int, int> &get_edge_id(); // Get the edge id
  const CFG_Node *get_src(); // Get the source node
  const CFG_Node *get_dst(); // Get the destination node
};

#endif
