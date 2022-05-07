#include "program.hh"

extern void cleanup(const char *);

Program::Program(list<Procedure *> *procedures) {
  this->procedures = procedures;
}

Program::~Program() { delete procedures; }

void Program::finish(bool viz) {
  cout << "Syntax check finished." << endl;
  basic_check();
  cout << "Semantic check finished." << endl;
  if (viz)
    visualize();
}

void Program::basic_check() {}

void Program::visualize() { cout << "Visualization finished." << endl; }
