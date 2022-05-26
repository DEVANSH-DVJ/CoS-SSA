#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;

SSA_Meta::SSA_Meta(pair<int, int> meta_num, list<SSA_Stmt *> *stmts) {
  this->meta_num = meta_num;

  this->stmts = stmts;
}

SSA_Meta::~SSA_Meta() {
  for (list<SSA_Stmt *>::iterator it = this->stmts->begin();
       it != this->stmts->end(); ++it) {
    delete *it;
  }
  delete this->stmts;
}

void SSA_Meta::visualize() const {
  for (list<SSA_Stmt *>::iterator it = this->stmts->begin();
       it != this->stmts->end(); ++it) {
    *dot_fd << (*it)->get_stmt();
    if (it != --this->stmts->end()) {
      *dot_fd << " \\n ";
    }
  }
}
