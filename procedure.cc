#include "procedure.hh"

extern void cleanup(const char *);

Procedure::Procedure(string name) {
  this->name = name;
  this->stmts = new list<Node *>();
}

Procedure::~Procedure() { delete this->stmts; }

void Procedure::basic_check() {}

void Procedure::visualize() {}
