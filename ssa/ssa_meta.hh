#ifndef _SSA_META_HH_
#define _SSA_META_HH_

#include <iostream>
#include <string>

#include <list>
#include <map>

using namespace std;

typedef enum {
  SSA_Meta_Start,
  SSA_Meta_End,
  SSA_Meta_Call,
  SSA_Meta_Input,
  SSA_Meta_Usevar,
  SSA_Meta_Expr,
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
