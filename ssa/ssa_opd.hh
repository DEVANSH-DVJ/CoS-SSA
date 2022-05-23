#ifndef _SSA_OPD_HH_
#define _SSA_OPD_HH_

#include <string>

typedef enum {
  SSA_VarOpd,
  SSA_NumOpd,
  SSA_InputOpd,
  SSA_UsevarOpd,
} SSA_OpdType;

class SSA_Opd {
public:
  SSA_OpdType type;
  int opd_id;
  int meta_id;

  int num_value;
  std::string var_name;

  // SSA_Opd(SSA_OpdType type, int opd_id, int meta_id);
  // ~SSA_Opd();
};

#endif
