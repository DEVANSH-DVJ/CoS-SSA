#ifndef _PROCEDURE_HH_
#define _PROCEDURE_HH_

#include <fstream>
#include <iostream>
#include <string>

#include <list>
#include <map>

#include "edge.hh"
#include "node.hh"

using namespace std;

class Procedure;

class Procedure {
public:
  string name;
  list<Node *> *nodes;
  map<int, Node *> *node_map;
  list<Edge *> *edges;

  Procedure();
  ~Procedure();

  void basic_check();
  void visualize();
};

#endif
