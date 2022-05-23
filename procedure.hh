#ifndef _PROCEDURE_HH_
#define _PROCEDURE_HH_

#include <map>
#include <utility>

#include <string>

class CFG_Edge;
class CFG_Node;
class SSA_Edge;
class SSA_Node;

class Procedure {
public:
  std::string name;

  std::map<int, CFG_Node *> *cfg_nodes;
  std::map<std::pair<int, int>, CFG_Edge *> *cfg_edges;

  std::map<int, SSA_Node *> *ssa_nodes;
  std::map<std::pair<int, int>, SSA_Edge *> *ssa_edges;

  Procedure(std::string name);
  ~Procedure();

  void visualize_cfg();
};

#endif
