#include "headers.hh"

using namespace std;

Program::Program() { this->procedures = new map<string, Procedure *>(); }

Program::~Program() {
  for (map<string, Procedure *>::iterator it = this->procedures->begin();
       it != this->procedures->end(); ++it) {
    delete it->second;
  }
  delete this->procedures;
}
