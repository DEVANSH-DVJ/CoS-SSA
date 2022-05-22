#ifndef _SSA_META_HH_
#define _SSA_META_HH_

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

  // SSA_Meta(SSA_MetaType type, int node_id, int meta_id);
  // ~SSA_Meta();
};

#endif
