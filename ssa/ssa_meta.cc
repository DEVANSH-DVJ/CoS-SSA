#include "../headers.hh"

using namespace std;

SSA_Meta::SSA_Meta(SSA_MetaType type, int node_id, int meta_id) {
  this->type = type;
  this->node_id = node_id;
  this->meta_id = meta_id;

  this->stmts = new list<SSA_Stmt *>();
}

SSA_Meta::~SSA_Meta() {
  for (list<SSA_Stmt *>::iterator it = this->stmts->begin();
       it != this->stmts->end(); ++it) {
    delete *it;
  }
  delete this->stmts;
}
