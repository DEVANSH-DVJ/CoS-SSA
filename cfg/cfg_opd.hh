#ifndef _CFG_OPD_HH_
#define _CFG_OPD_HH_

#include <string>

typedef enum {
  CFG_NumOpd,
  CFG_VarOpd,
  CFG_InputOpd,
  CFG_UsevarOpd,
} CFG_OpdType;

class CFG_Opd {
public:
  /* Always initialized: by constructor */
  CFG_OpdType type;

  /* Conditionally initialized: post parsing */
  int num_value;
  std::string var_name;

  /* Constructors and Destructor */
  CFG_Opd(CFG_OpdType type);
  CFG_Opd(CFG_OpdType type, int num_value);
  CFG_Opd(CFG_OpdType type, std::string var_name);
  ~CFG_Opd();

  /* Get functions */
  // Get the node type
  const CFG_OpdType get_type() const;
  // Get opd value if it is a number
  const int get_opd_value() const;
  // Get opd value if it is a variable
  const std::string &get_opd_var() const;

  /* Helper functions */
  // Return the string rep
  std::string str() const;
};

#endif
