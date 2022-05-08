#ifndef _OPD_HH_
#define _OPD_HH_

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

typedef enum { int_opd, var_opd } OpdType;

class Opd;

class IntOpd;
class VarOpd;

class Opd {
public:
  OpdType type;

  Opd(OpdType type);
  ~Opd();

  virtual string to_str() = 0;
};

class IntOpd : public Opd {
public:
  int value;

  IntOpd(int value);
  ~IntOpd();

  string to_str();
};

class VarOpd : public Opd {
public:
  string name;

  VarOpd(string name);
  ~VarOpd();

  string to_str();
};

#endif
