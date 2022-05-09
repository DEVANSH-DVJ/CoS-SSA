#include "program.hh"

extern string dot_file;
extern string png_file;
extern fstream *dot_fd;
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
    system(("dot -Tpng " + dot_file + " -o " + png_file).c_str());
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
  *dot_fd << "digraph G {" << endl;
  for (list<Procedure *>::iterator it = procedures->begin();
       it != procedures->end(); ++it) {
    (*it)->visualize();
  }
  *dot_fd << "}" << endl;
}
