#ifndef _SSA_META_HH_
#define _SSA_META_HH_

#include <list>
#include <utility>

class SSA_Stmt;

class SSA_Meta {
public:
  /* Always initialized: by constructor */
  std::pair<int, int> meta_num;

  /* Always initialized: post parsing*/
  std::list<SSA_Stmt *> *stmts;

  /* Constructors and Destructor */
  SSA_Meta(std::pair<int, int> meta_num, std::list<SSA_Stmt *> *stmts);
  ~SSA_Meta();

  /* Helper functions */
  // Visualize the meta
  void visualize() const;
};

#endif
