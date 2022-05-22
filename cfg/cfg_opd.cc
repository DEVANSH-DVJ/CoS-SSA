#include "../headers.hh"

CFG_Opd::CFG_Opd(CFG_OpdType type, int num_value) {
  this->type = type;

  CHECK_INVARIANT(type == CFG_Num, "CFG_Num is expected");

  this->num_value = num_value;
  this->var_name = "";
  this->str = to_string(num_value);
}

CFG_Opd::CFG_Opd(CFG_OpdType type, std::string var_name) {
  this->type = type;

  CHECK_INVARIANT(type == CFG_Var, "CFG_Var is expected");

  this->num_value = 0;
  this->var_name = var_name;
  this->str = var_name;
}

CFG_Opd::~CFG_Opd() {}
