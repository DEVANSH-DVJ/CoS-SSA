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

  CFG_Opd(CFG_OpdType type);
  ~CFG_Opd();

  void to_string();
};

#endif
