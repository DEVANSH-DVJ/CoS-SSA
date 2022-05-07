#ifndef _PROCEDURE_HH_
#define _PROCEDURE_HH_

#include <fstream>
#include <iostream>
#include <string>

#include <list>

#include "node.hh"

using namespace std;

class Procedure;

class Procedure {
public:
  string name;
  list<Node *> *stmts;

  Procedure(string name);
  ~Procedure();

  void basic_check();
  void visualize();
};

#endif
