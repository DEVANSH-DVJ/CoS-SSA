#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;

SSA_Meta::SSA_Meta(int node_id, int meta_id) {
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

void SSA_Meta::visualize() {
  for (list<SSA_Stmt *>::iterator it = this->stmts->begin();
       it != this->stmts->end(); ++it) {
    *dot_fd << (*it)->get_stmt();
    if (it != --this->stmts->end()) {
      *dot_fd << " \\n ";
    }
  }
}
