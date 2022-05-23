#include "../headers.hh"

using namespace std;

SSA_Stmt::SSA_Stmt(SSA_StmtType type) {
  this->type = type;

  this->stmt = "";

  this->callee_proc = "";
  this->op = "";
  this->lopd = NULL;
  this->ropd1 = NULL;
  this->ropd2 = NULL;
  this->ropds = NULL;
}

SSA_Stmt::~SSA_Stmt() {
  if (this->lopd != NULL)
    delete this->lopd;

  if (this->ropd1 != NULL)
    delete this->ropd1;

  if (this->ropd2 != NULL)
    delete this->ropd2;

  if (this->ropds != NULL) {
    for (list<SSA_Opd *>::iterator it = this->ropds->begin();
         it != this->ropds->end(); ++it)
      delete *it;

    delete this->ropds;
  }
}
