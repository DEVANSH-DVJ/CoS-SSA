#include "program.hh"

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
  cout << "Syntax check finished." << endl;
  basic_check();
  cout << "Semantic check finished." << endl;
  if (viz)
    visualize();
}

void Program::basic_check() {
  for (list<Procedure *>::iterator it = procedures->begin();
       it != procedures->end(); ++it) {
    (*it)->basic_check();
    proc_names->push_back((*it)->name);
  }
}

void Program::visualize() { cout << "Visualization finished." << endl; }
