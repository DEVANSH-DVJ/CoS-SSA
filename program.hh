#ifndef _PROGRAM_HH_
#define _PROGRAM_HH_

#include "ddg/ddg_context.hh"

#include <list>
#include <map>
#include <set>
#include <utility>

#include <string>

class Procedure;
class CFG_Edge;
class CFG_Node;
typedef struct QDef QDef;
class ContextTable;
class SSA_Edge;
class SSA_Node;

class Program;

class Program {
  /* State */
  std::string tool;
  std::string input_name;

  /* Procedures */
  std::list<Procedure *> *procs;
  std::map<std::string, Procedure *> *procedures;

  /* CFG Graph */
  std::map<int, CFG_Node *> *cfg_nodes;
  std::map<std::pair<int, int>, CFG_Edge *> *cfg_edges;

  /* DDG */
  std::set<QDef> ddg_nodes;
  ContextTable ddg_context_table;
  std::map<QDef, std::set<QDef>> ddg_edges;
  std::map<QDef, std::set<QDef>> ddg_reverse_edges;
  std::map<QDef, int> ddg_propagated_values;
  std::map<QNode, int> ddg_context_transitions;
  std::map<int, std::set<QNode>> ddg_reverse_context_transitions;

  /* SSA Graph */
  std::map<int, SSA_Node *> *ssa_nodes;
  std::map<std::pair<int, int>, SSA_Edge *> *ssa_edges;

  /* Helper functions */
  // Parse CFG graph
  void parse_cfg();
  // Parse SSA graph
  void parse_ssa();
  // Construct DDG from the CFG graph
  void construct_ddg();
  // Do constant propagation on the DDG
  void propagate_ddg_constants();
  // Visualize CFG graph
  void visualize_cfg();
  // Visualize SSA graph
  void visualize_ssa();
  // Visualize the DDG
  void visualize_ddg();

public:
  /* Constructors and Destructor */
  Program(std::string tool, std::string input_file);
  ~Program();

  /* Get functions */
  // Get procedures
  std::map<std::string, Procedure *> *get_procs();
  // Get procedure by name
  Procedure *get_proc(std::string name);
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

  std::set<std::string> get_globals();
  std::set<QDef> get_ddg_nodes();
  std::set<QDef> get_ddg_incoming(QDef node);
  std::set<QDef> get_ddg_outgoing(QDef node);
  bool create_ddg_transition(QNode from_qnode, const Context& to_context);
  std::map<QNode, int>::iterator get_ddg_transition(QNode qnode);
  std::map<QNode, int>::iterator ddg_transitions_end();
  std::map<int, std::set<QNode>>::iterator get_ddg_reverse_transitions(int context);
  std::map<int, std::set<QNode>>::iterator ddg_reverse_transitions_end();
  int insert_ddg_context(Context context);
  void add_ddg_node(QDef node);
  void add_ddg_edge(QDef src, QDef dest);

  // Cleanup
  void cleanup();

  // Run
  void run();
};

#endif
