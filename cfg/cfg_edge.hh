#ifndef _CFG_EDGE_HH_
#define _CFG_EDGE_HH_

#include <utility>

class CFG_Node;

class CFG_Edge;

class CFG_Edge {
  /* Always initialized: by constructor */
  std::pair<int, int> edge_id;
  CFG_Node *src;
  CFG_Node *dst;

public:
  /* Constructors and Destructor */
  CFG_Edge(int src_id, int dst_id);
  ~CFG_Edge();

  /* Get functions */
  // Get the edge id
  const std::pair<int, int> &get_edge_id() const;
  // Get the source node
  const CFG_Node *get_src() const;
  // Get the destination node
  const CFG_Node *get_dst() const;

  /* Helper functions */
  // Visualize the edge
  void visualize() const;
};

#endif
