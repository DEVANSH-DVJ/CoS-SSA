#include "../headers.hh"

using namespace std;

SSA_Opd::SSA_Opd(SSA_OpdType type, int node_id, int meta_id) {
  this->type = type;
  this->node_id = node_id;
  this->meta_id = meta_id;

  if (type == SSA_InputOpd) {
    this->str = "INPUT_" + to_string(node_id);
  } else if (type == SSA_UsevarOpd) {
    this->str = "USEVAR_" + to_string(node_id) + "_" + to_string(meta_id);
  } else {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,
                    "SSA_VarOpd or SSA_NumOpd expected");
  }

  this->num_value = 0;
  this->var_name = "";
}

SSA_Opd::SSA_Opd(SSA_OpdType type, int node_id, int meta_id, int num_value) {
  this->type = type;
  this->node_id = node_id;
  this->meta_id = meta_id;

  CHECK_INVARIANT(type == SSA_NumOpd, "SSA_NumOpd expected");
  this->str = to_string(num_value);

  this->num_value = num_value;
  this->var_name = "";
}

SSA_Opd::SSA_Opd(SSA_OpdType type, int node_id, int meta_id, string var_name) {
  this->type = type;
  this->node_id = node_id;
  this->meta_id = meta_id;

  CHECK_INVARIANT(type == SSA_VarOpd, "SSA_VarOpd expected");
  this->str = var_name + "_" + to_string(node_id) + "_" + to_string(meta_id);

  this->num_value = 0;
  this->var_name = var_name;
}

SSA_Opd::~SSA_Opd() {}
