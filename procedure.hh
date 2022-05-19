#ifndef _PROCEDURE_HH_
#define _PROCEDURE_HH_

#include <iostream>
#include <string>

#include <map>

using namespace std;

#include "error.hh"

class Procedure {
public:
  string name;

  Procedure(string name);
  ~Procedure();
};

#endif
