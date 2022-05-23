#ifndef _SSA_OPD_HH_
#define _SSA_OPD_HH_

#include <string>

typedef enum {
  SSA_NumOpd,
  SSA_VarOpd,
  SSA_InputOpd,
  SSA_UsevarOpd,
} SSA_OpdType;

class SSA_Opd {
public:
  /* Always initialized: by constructor */
  SSA_OpdType type;
  int node_id;
  int meta_id;
  std::string str;

  /* Conditionally initialized: post parsing */
  int num_value;
  std::string var_name;

  /* Constructors and Destructor */
  SSA_Opd(SSA_OpdType type, int node_id, int meta_id);
  SSA_Opd(SSA_OpdType type, int node_id, int meta_id, int num_value);
  SSA_Opd(SSA_OpdType type, int node_id, int meta_id, std::string var_name);
  ~SSA_Opd();
};

#endif
