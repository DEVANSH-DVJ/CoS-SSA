#ifndef _PROGRAM_HH_
#define _PROGRAM_HH_

#include <list>
#include <map>
#include <utility>

class Procedure;
class CFG_Edge;
class CFG_Node;
class SSA_Edge;
class SSA_Node;

class Program;

class Program {
  /* Procedures */
  std::list<Procedure *> *procs;
  std::map<string, Procedure *> *procedures;

  /* CFG Graph */
  std::map<int, CFG_Node *> *cfg_nodes;
  std::map<std::pair<int, int>, CFG_Edge *> *cfg_edges;

  /* SSA Graph */
  std::map<int, SSA_Node *> *ssa_nodes;
  std::map<std::pair<int, int>, SSA_Edge *> *ssa_edges;

public:
  /* Constructors and Destructor */
  Program();
  ~Program();

  /* Get functions */
  // Get procedures
  std::map<std::string, Procedure *> *get_procs();
  // Get procedure by name
  Procedure *get_proc(string name);
  // Get CFG node by id
  CFG_Node *get_cfg_node(int node_id, bool abort_if_not_found);
  // Get SSA node by id
  SSA_Node *get_ssa_node(int node_id, bool abort_if_not_found);

  /* Update functions */
  // Add a procedure
  void add_proc(Procedure *proc);
  // Push a procedure in the list
  void push_proc(Procedure *proc);
  // Add a CFG node
  void add_cfg_node(CFG_Node *node);
  // Add a CFG edge
  void add_cfg_edge(CFG_Edge *edge);
  // Add a SSA node
  void add_ssa_node(SSA_Node *node);
  // Add a SSA edge
  void add_ssa_edge(SSA_Edge *edge);

  /* Helper functions */
  // Visualize CFG graph
  void visualize_cfg();
  // Visualize SSA graph
  void visualize_ssa();
};

#endif
