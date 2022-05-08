#include "opd.hh"

extern void cleanup(const char *);

Opd::Opd(OpdType type) { this->type = type; }
Opd::~Opd() {}

IntOpd::IntOpd(int value) : Opd(int_opd) { this->value = value; }
IntOpd::~IntOpd() {}
string IntOpd::to_str() { return to_string(this->value); }

VarOpd::VarOpd(string name) : Opd(var_opd) { this->name = name; }
VarOpd::~VarOpd() {}
string VarOpd::to_str() { return this->name; }
