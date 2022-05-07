#ifndef _PROGRAM_HH_
#define _PROGRAM_HH_

#include <fstream>
#include <iostream>
#include <string>

#include <list>

#include "procedure.hh"

using namespace std;

class Program;

class Program {
public:
  list<Procedure *> *procedures;

  Program(list<Procedure *> *procedures);
  ~Program();

  void finish(bool viz);
  void basic_check();
  void visualize();
};

#endif
