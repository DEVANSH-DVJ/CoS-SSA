#ifndef _SSA_EDGE_HH_
#define _SSA_EDGE_HH_

class SSA_Node;

class SSA_Edge {
public:
  /* Always initialized: by constructor */
  std::pair<int, int> edge_id;
  SSA_Node *src;
  SSA_Node *dst;

  /* Constructors and Destructor */
  SSA_Edge(int src_id, int dst_id);
  ~SSA_Edge();

  /* Helper functions */
  void visualize();
};

#endif
