#include "program.hh"

extern fstream *gv_fd;
extern void cleanup(const char *);

Program::Program(list<Procedure *> *procedures) {
  this->procedures = procedures;
  this->proc_names = new list<string>();
}

Program::~Program() {
  delete procedures;
  delete proc_names;
}

void Program::finish(bool viz) {
  basic_check();
  if (viz) {
    visualize();
  }
}

void Program::basic_check() {
  for (list<Procedure *>::iterator it = procedures->begin();
       it != procedures->end(); ++it) {
    (*it)->basic_check();
    proc_names->push_back((*it)->name);
  }
}

void Program::visualize() {
  *gv_fd << "digraph G {" << endl;
  for (list<Procedure *>::iterator it = procedures->begin();
       it != procedures->end(); ++it) {
    (*it)->visualize();
  }
  *gv_fd << "}" << endl;
}
