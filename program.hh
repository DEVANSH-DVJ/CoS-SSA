#ifndef _PROGRAM_HH_
#define _PROGRAM_HH_

#include <list>
#include <map>
#include <utility>

class Procedure;
class CFG_Edge;
class CFG_Node;

class Program {
public:
  std::list<Procedure *> *procs;
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
