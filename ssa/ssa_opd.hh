#ifndef _SSA_OPD_HH_
#define _SSA_OPD_HH_

#include <string>

#include <utility>

typedef enum {
  SSA_NumOpd,
  SSA_VarOpd,
  SSA_InputOpd,
  SSA_UsevarOpd,
} SSA_OpdType;

class SSA_Opd {
  /* Always initialized: by constructor */
  SSA_OpdType type;
  int node_id;
  int meta_id;

  /* Conditionally initialized: post parsing */
  int num_value;
  std::string var_name;

public:
  /* Constructors and Destructor */
  SSA_Opd(SSA_OpdType type, int node_id, int meta_id);
  SSA_Opd(SSA_OpdType type, int node_id, int meta_id, int num_value);
  SSA_Opd(SSA_OpdType type, int node_id, int meta_id, std::string var_name);
  ~SSA_Opd();

  /* Get functions */
  // Get the node type
  const SSA_OpdType get_type() const;
  // Get the node id
  const std::pair<int, int> get_meta_num() const;
  // Get opd value if it is a number
  const int get_opd_value() const;
  // Get opd value if it is a variable
  const std::string &get_opd_var() const;

  /* Helper functions */
  // Return the string rep
  std::string str() const;
};

#endif
