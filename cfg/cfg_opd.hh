#ifndef _CFG_OPD_HH_
#define _CFG_OPD_HH_

#include <string>

typedef enum {
  CFG_NumOpd,
  CFG_VarOpd,
} CFG_OpdType;

class CFG_Opd {
public:
  /* Always initialized: by constructor */
  CFG_OpdType type;
  std::string str;

  /* Conditionally initialized: post parsing */
  int num_value;
  std::string var_name;

  /* Constructors and Destructor */
  CFG_Opd(CFG_OpdType type, int num_value);
  CFG_Opd(CFG_OpdType type, std::string var_name);
  ~CFG_Opd();
};

#endif
