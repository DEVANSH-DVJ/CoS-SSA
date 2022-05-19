#ifndef _PROGRAM_HH_
#define _PROGRAM_HH_

#include <iostream>
#include <string>

#include <map>

using namespace std;

#include "error.hh"
#include "procedure.hh"

class Program {
public:
  map<string, Procedure *> procedures;

  Program();
  ~Program();
};

#endif
