#include "../headers.hh"

CFG_Opd::CFG_Opd(CFG_OpdType type) {
  this->type = type;

  if (type == CFG_InputOpd) {
    this->str = "INPUT";
  } else if (type == CFG_UsevarOpd) {
    this->str = "USEVAR";
  } else {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,
                    "CFG_InputOpd or CFG_UsevarOpd expected");
  }

  this->num_value = 0;
  this->var_name = "";
}

CFG_Opd::CFG_Opd(CFG_OpdType type, int num_value) {
  this->type = type;

  CHECK_INVARIANT(type == CFG_NumOpd, "CFG_NumOpd expected");
  this->str = to_string(num_value);

  this->num_value = num_value;
  this->var_name = "";
}

CFG_Opd::CFG_Opd(CFG_OpdType type, std::string var_name) {
  this->type = type;

  CHECK_INVARIANT(type == CFG_VarOpd, "CFG_VarOpd expected");
  this->str = var_name;

  this->num_value = 0;
  this->var_name = var_name;
}

CFG_Opd::~CFG_Opd() {}
