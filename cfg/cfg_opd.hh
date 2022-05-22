#ifndef _CFG_OPD_HH_
#define _CFG_OPD_HH_

#include <string>

typedef enum {
  CFG_Num,
  CFG_Var,
} CFG_OpdType;

class CFG_Opd {
public:
  CFG_OpdType type;
  std::string str;

  int num_value;
  std::string var_name;

  CFG_Opd(CFG_OpdType type, int num_value);
  CFG_Opd(CFG_OpdType type, std::string var_name);
  ~CFG_Opd();
};

#endif
