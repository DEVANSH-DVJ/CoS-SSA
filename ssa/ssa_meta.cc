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

const pair<int, int> SSA_Meta::get_meta_num() const { return this->meta_num; }

const list<SSA_Stmt *> *SSA_Meta::get_stmts() const { return this->stmts; }

void SSA_Meta::visualize() const {
  for (list<SSA_Stmt *>::iterator it = this->stmts->begin();
       it != this->stmts->end(); ++it) {
    *dot_fd << (*it)->get_stmt();
    if (it != --this->stmts->end()) {
      *dot_fd << " \\n ";
    }
  }
}
