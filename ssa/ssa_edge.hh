#ifndef _SSA_EDGE_HH_
#define _SSA_EDGE_HH_

class SSA_Node;

class SSA_Edge {
  /* Always initialized: by constructor */
  std::pair<int, int> edge_id;
  SSA_Node *src;
  SSA_Node *dst;

public:
  /* Constructors and Destructor */
  SSA_Edge(int src_id, int dst_id);
  ~SSA_Edge();

  /* Get functions */
  // Get the edge id
  const std::pair<int, int> &get_edge_id() const;
  // Get the source node
  const SSA_Node *get_src() const;
  // Get the destination node
  const SSA_Node *get_dst() const;

  /* Helper functions */
  // Visualize the edge
  void visualize() const;
};

#endif
