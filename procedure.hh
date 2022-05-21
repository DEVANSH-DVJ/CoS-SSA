#ifndef _PROCEDURE_HH_
#define _PROCEDURE_HH_

#include <map>
#include <utility>

#include <string>

#include "cfg/cfg_edge.hh"
#include "cfg/cfg_node.hh"

class Procedure {
public:
  std::string name;

  std::map<int, CFG_Node *> *cfg_nodes;
  std::map<std::pair<int, int>, CFG_Edge *> *cfg_edges;

  Procedure(std::string name);
  ~Procedure();

  void visualize_cfg();
};

#endif
