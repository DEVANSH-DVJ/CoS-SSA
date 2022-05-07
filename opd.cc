#include "opd.hh"

extern void cleanup(const char *);

Opd::Opd(OpdType type) { this->type = type; }
Opd::~Opd() {}

IntOpd::IntOpd(int value) : Opd(int_opd) { this->value = value; }
IntOpd::~IntOpd() {}
void IntOpd::visualize() { cout << value; }

VarOpd::VarOpd(string name) : Opd(var_opd) { this->name = name; }
VarOpd::~VarOpd() {}
void VarOpd::visualize() { cout << name; }
