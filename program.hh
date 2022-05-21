#ifndef _PROGRAM_HH_
#define _PROGRAM_HH_

#include <map>

#include "error.hh"
#include "procedure.hh"

class Program {
public:
  map<string, Procedure *> *procedures;

  Program();
  ~Program();

  Procedure *get_proc(string name);
};

#endif
