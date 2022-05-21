#ifndef _PROGRAM_HH_
#define _PROGRAM_HH_

#include <map>
#include <utility>

#include "error.hh"
#include "procedure.hh"

#include "cfg/cfg_edge.hh"
#include "cfg/cfg_node.hh"

class Program {
public:
  std::map<string, Procedure *> *procedures;

  std::map<int, CFG_Node *> *cfg_nodes;
  std::map<std::pair<int, int>, CFG_Edge *> *cfg_edges;

  Program();
  ~Program();

  Procedure *get_proc(string name);
  CFG_Node *get_cfg_node(int node_id);

  void visualize_cfg();
};

#endif
