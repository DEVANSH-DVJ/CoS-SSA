#ifndef _SSA_META_HH_
#define _SSA_META_HH_

#include <utility>

class SSA_Stmt;

typedef enum {
  SSA_StartMeta,
  SSA_EndMeta,
  SSA_CallMeta,
  SSA_InputMeta,
  SSA_UsevarMeta,
  SSA_ExprMeta,
} SSA_MetaType;

class SSA_Meta {
public:
  SSA_MetaType type;
  int node_id;
  int meta_id;

  std::list<SSA_Stmt*> *stmts;

  // SSA_Meta(SSA_MetaType type, int node_id, int meta_id);
  // ~SSA_Meta();
};

#endif
