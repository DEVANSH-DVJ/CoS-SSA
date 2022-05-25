#include "../headers.hh"

using namespace std;

CFG_Opd::CFG_Opd(CFG_OpdType type) {
  this->type = type;

  if (type != CFG_InputOpd && type != CFG_UsevarOpd) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,
                    "CFG_InputOpd or CFG_UsevarOpd expected");
  }

  this->num_value = 0;
  this->var_name = "";
}

CFG_Opd::CFG_Opd(CFG_OpdType type, int num_value) {
  this->type = type;

  CHECK_INVARIANT(type == CFG_NumOpd, "CFG_NumOpd expected");

  this->num_value = num_value;
  this->var_name = "";
}

CFG_Opd::CFG_Opd(CFG_OpdType type, std::string var_name) {
  this->type = type;

  CHECK_INVARIANT(type == CFG_VarOpd, "CFG_VarOpd expected");

  this->num_value = 0;
  this->var_name = var_name;
}

CFG_Opd::~CFG_Opd() {}

const CFG_OpdType CFG_Opd::get_type() const { return this->type; }

const int CFG_Opd::get_opd_value() const {
  CHECK_INVARIANT(this->type == CFG_NumOpd, "CFG_NumOpd expected");
  return this->num_value;
}

const string &CFG_Opd::get_opd_var() const {
  CHECK_INVARIANT(this->type == CFG_VarOpd, "CFG_VarOpd expected");
  return this->var_name;
}

string CFG_Opd::str() const {
  switch (type) {
  case (CFG_NumOpd):
    return to_string(num_value);
  case (CFG_VarOpd):
    return var_name;
  case (CFG_InputOpd):
    return "INPUT";
  case (CFG_UsevarOpd):
    return "USEVAR";
  }
  CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "CFG_OpdType unknown");
  return "";
}
