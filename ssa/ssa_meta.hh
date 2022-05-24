#ifndef _SSA_META_HH_
#define _SSA_META_HH_

#include <utility>

class SSA_Stmt;

class SSA_Meta {
public:
  /* Always initialized: by constructor */
  int node_id;
  int meta_id;

  /* Always initialized: post parsing*/
  std::list<SSA_Stmt *> *stmts;

  /* Constructors and Destructor */
  SSA_Meta(int node_id, int meta_id);
  ~SSA_Meta();

  void visualize();
};

#endif
