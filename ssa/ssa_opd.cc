#include "../headers.hh"

using namespace std;

SSA_Opd::SSA_Opd(SSA_OpdType type, pair<int, int> meta_num) {
  if (type != SSA_InputOpd && type != SSA_UsevarOpd) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,
                    "SSA_InputOpd or SSA_UsevarOpd expected");
  }

  this->type = type;
  this->meta_num = meta_num;

  this->num_value = 0;
  this->var_name = "";
}

SSA_Opd::SSA_Opd(SSA_OpdType type, int num_value) {
  CHECK_INVARIANT(type == SSA_NumOpd, "SSA_NumOpd expected");

  this->type = type;
  this->meta_num = make_pair(0, 0);

  this->num_value = num_value;
  this->var_name = "";
}

SSA_Opd::SSA_Opd(SSA_OpdType type, pair<int, int> meta_num, string var_name) {
  CHECK_INVARIANT(type == SSA_VarOpd, "SSA_VarOpd expected");

  this->type = type;
  this->meta_num = meta_num;

  this->num_value = 0;
  this->var_name = var_name;
}

SSA_Opd::~SSA_Opd() {}

const SSA_OpdType SSA_Opd::get_type() const { return this->type; }

const pair<int, int> SSA_Opd::get_meta_num() const { return this->meta_num; }

const int SSA_Opd::get_opd_value() const { return this->num_value; }

const string &SSA_Opd::get_opd_var() const { return this->var_name; }

string SSA_Opd::str() const {
  switch (type) {
  case (SSA_NumOpd):
    return to_string(num_value);
  case (SSA_VarOpd):
    return var_name + "_" + to_string(meta_num.first) + "_" +
           to_string(meta_num.second);
  case (SSA_InputOpd):
    return "INPUT_" + to_string(meta_num.first);
  case (SSA_UsevarOpd):
    return "USEVAR_" + to_string(meta_num.first) + "_" +
           to_string(meta_num.second);
  }

  CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "SSA_OpdType unknown");

  return "";
}
