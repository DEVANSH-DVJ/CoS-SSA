%{

  #include <iostream>
  #include <string>

  using namespace std;

  #include "../headers.hh"

  extern int ssa_lex(void);

  extern int ssa_lineno;
  extern int ssa_error(const char *);

  extern Program *program;

%}
%union{
  string *name;
  int value;
  pair<int, int> *meta_num;

  Procedure *proc;

  list<SSA_Node *> *ssa_node_list;
  list<SSA_Edge *> *ssa_edge_list;
  list<SSA_Opd *> *ssa_opd_list;

  SSA_Node *ssa_node;
  SSA_Edge *ssa_edge;
  SSA_Stmt *ssa_stmt;
  SSA_Opd *ssa_opd;
}

%define api.prefix {ssa_}

%token SSA_EOS
%token SSA_COLON
%token SSA_ARROW
%token SSA_COMMA
%token SSA_LCB
%token SSA_RCB
%token SSA_LRB
%token SSA_RRB
%token SSA_UNDERSCORE

%token SSA_START
%token SSA_END
%token SSA_CALL
%token SSA_INPUT
%token SSA_USEVAR
%token SSA_PHI

%token SSA_ASSIGN
%token <name> SSA_OP

%token <name> SSA_ID
%token <value> SSA_NUM

%token SSA_UNKNOWN

%left '+' '-'
%left '*' '/'

%type <proc> Proc

%type <ssa_node_list> NodeList
%type <ssa_edge_list> EdgeList

%type <ssa_node> StartNode
%type <ssa_node> EndNode
%type <ssa_node> Node
%type <ssa_node> CallNode
%type <ssa_node> InputNode
%type <ssa_node> UsevarNode
%type <ssa_node> ExprNode
%type <ssa_stmt> PhiStmt
%type <ssa_opd_list> VarList

%type <ssa_edge> Edge

%type <ssa_opd> Opd
%type <ssa_opd> Var
%type <meta_num> MetaNum

%%

/* Program */
Program
  : ProcDefList SSA_EOS ProcList
;

ProcDefList
  : SSA_ID
  {
    program->procedures->insert(make_pair(*$1, new Procedure(*$1)));
  }
  | ProcDefList SSA_COMMA SSA_ID
  {
    program->procedures->insert(make_pair(*$3, new Procedure(*$3)));
  }
;

ProcList
  : Proc
  {
    program->procs->push_back($1);
  }
  | ProcList Proc
  {
    program->procs->push_back($2);
  }
;

/* Procedure */

Proc
  : SSA_LCB StartNode NodeList EndNode EdgeList SSA_RCB
  {
    string proc_name = $2->parent_proc;
    CHECK_INPUT_AND_ABORT(proc_name == $4->parent_proc,
                          "Start node and end node must be in the same procedure");

    Procedure *proc = program->get_proc(proc_name);
    proc->ssa_nodes->insert(make_pair($2->node_id, $2));
    proc->ssa_nodes->insert(make_pair($4->node_id, $4));
    for (list<SSA_Node *>::iterator it = $3->begin(); it != $3->end(); ++it) {
      proc->ssa_nodes->insert(make_pair((*it)->node_id, *it));
      (*it)->set_parent_proc(proc_name);
    }
    for (list<SSA_Edge *>::iterator it = $5->begin(); it != $5->end(); ++it)
      proc->ssa_edges->insert(make_pair((*it)->get_edge_id(), *it));

    $$ = proc;
  }
;

NodeList
  : Node
  {
    $$ = new list<SSA_Node *>();
    $$->push_back($1);
  }
  | NodeList Node
  {
    $$ = $1;
    $$->push_back($2);
  }
;

EdgeList
  : Edge
  {
    $$ = new list<SSA_Edge *>();
    $$->push_back($1);
  }
  | EdgeList Edge
  {
    $$ = $1;
    $$->push_back($2);
  }
;

/* Statements */

StartNode
  : MetaNum SSA_COLON SSA_START SSA_ID SSA_EOS
  {
    string stmt = "START " + *$4;
    SSA_Node *node = new SSA_Node(SSA_StartNode, $1->first, stmt);
    node->set_parent_proc(*$4);

    program->ssa_nodes->insert(make_pair($1->first, node));

    $$ = node;
  }
;

EndNode
  : MetaNum SSA_COLON SSA_END SSA_ID SSA_EOS
  {
    string stmt = "END " + *$4;
    SSA_Node *node = new SSA_Node(SSA_EndNode, $1->first, stmt);
    node->set_parent_proc(*$4);

    program->ssa_nodes->insert(make_pair($1->first, node));

    $$ = node;
  }
;

Node
  : CallNode
  {
    $$ = $1;
  }
  | InputNode
  {
    $$ = $1;
  }
  | UsevarNode
  {
    $$ = $1;
  }
  | ExprNode
  {
    $$ = $1;
  }
;

CallNode
  : MetaNum SSA_COLON SSA_CALL SSA_ID SSA_EOS
  {
    string stmt = "CALL " + *$4;
    SSA_Node *node = new SSA_Node(SSA_CallNode, $1->first, stmt, *$4);

    program->ssa_nodes->insert(make_pair($1->first, node));

    $$ = node;
  }
;

InputNode
  : MetaNum SSA_COLON Var SSA_ASSIGN SSA_INPUT SSA_EOS
  {
    SSA_Node *node = new SSA_Node(SSA_AssignNode, $1->first);

    SSA_Stmt *stmt = new SSA_Stmt(SSA_AssignStmt, "=",
                                  $3,
                                  new SSA_Opd(SSA_InputOpd, *$1),
                                  NULL);

    SSA_Meta *meta = new SSA_Meta(*$1, new list<SSA_Stmt *>({stmt}));

    node->add_meta(meta, $1->second);
    program->ssa_nodes->insert(make_pair($1->first, node));

    $$ = node;
  }
;

UsevarNode
  : MetaNum SSA_COLON SSA_USEVAR SSA_ASSIGN Var SSA_EOS
  {
    SSA_Node *node = program->get_ssa_node($1->first, false);
    if (node == NULL)
      node = new SSA_Node(SSA_AssignNode, $1->first);

    SSA_Stmt *stmt = new SSA_Stmt(SSA_AssignStmt, "=",
                                  new SSA_Opd(SSA_UsevarOpd, *$1),
                                  $5,
                                  NULL);

    SSA_Meta *meta = new SSA_Meta(*$1, new list<SSA_Stmt *>({stmt}));

    node->add_meta(meta, $1->second);
    program->ssa_nodes->insert(make_pair($1->first, node));

    $$ = node;
  }
  | MetaNum SSA_COLON PhiStmt SSA_USEVAR SSA_ASSIGN Var SSA_EOS
  {
    SSA_Node *node = program->get_ssa_node($1->first, false);
    if (node == NULL)
      node = new SSA_Node(SSA_AssignNode, $1->first);

    SSA_Stmt *stmt = new SSA_Stmt(SSA_AssignStmt, "=",
                                  new SSA_Opd(SSA_UsevarOpd, *$1),
                                  $6,
                                  NULL);

    SSA_Meta *meta = new SSA_Meta(*$1, new list<SSA_Stmt *>({$3, stmt}));

    node->add_meta(meta, $1->second);
    program->ssa_nodes->insert(make_pair($1->first, node));

    $$ = node;
  }
;

ExprNode
  : MetaNum SSA_COLON Var SSA_ASSIGN Opd SSA_OP Opd SSA_EOS
  {
    SSA_Node *node = program->get_ssa_node($1->first, false);
    if (node == NULL)
      node = new SSA_Node(SSA_AssignNode, $1->first);

    SSA_Stmt *stmt = new SSA_Stmt(SSA_AssignStmt, *$6, $3, $5, $7);

    SSA_Meta *meta = new SSA_Meta(*$1, new list<SSA_Stmt *>({stmt}));

    node->add_meta(meta, $1->second);
    program->ssa_nodes->insert(make_pair($1->first, node));

    $$ = node;
  }
  | MetaNum SSA_COLON PhiStmt Var SSA_ASSIGN Opd SSA_OP Opd SSA_EOS
  {
    SSA_Node *node = program->get_ssa_node($1->first, false);
    if (node == NULL)
      node = new SSA_Node(SSA_AssignNode, $1->first);

    SSA_Stmt *stmt = new SSA_Stmt(SSA_AssignStmt, *$7, $4, $6, $8);

    SSA_Meta *meta = new SSA_Meta(*$1, new list<SSA_Stmt *>({$3, stmt}));

    node->add_meta(meta, $1->second);
    program->ssa_nodes->insert(make_pair($1->first, node));

    $$ = node;
  }
  | MetaNum SSA_COLON PhiStmt PhiStmt Var SSA_ASSIGN Opd SSA_OP Opd SSA_EOS
  {
    SSA_Node *node = program->get_ssa_node($1->first, false);
    if (node == NULL)
      node = new SSA_Node(SSA_AssignNode, $1->first);

    SSA_Stmt *stmt = new SSA_Stmt(SSA_AssignStmt, *$8, $5, $7, $9);

    SSA_Meta *meta = new SSA_Meta(*$1, new list<SSA_Stmt *>({$3, $4, stmt}));

    node->add_meta(meta, $1->second);
    program->ssa_nodes->insert(make_pair($1->first, node));

    $$ = node;
  }
  | MetaNum SSA_COLON Var SSA_ASSIGN Opd SSA_EOS
  {
    SSA_Node *node = program->get_ssa_node($1->first, false);
    if (node == NULL)
      node = new SSA_Node(SSA_AssignNode, $1->first);

    SSA_Stmt *stmt = new SSA_Stmt(SSA_AssignStmt, "=", $3, $5, NULL);

    SSA_Meta *meta = new SSA_Meta(*$1, new list<SSA_Stmt *>({stmt}));

    node->add_meta(meta, $1->second);
    program->ssa_nodes->insert(make_pair($1->first, node));

    $$ = node;
  }
  | MetaNum SSA_COLON PhiStmt Var SSA_ASSIGN Opd SSA_EOS
  {
    SSA_Node *node = program->get_ssa_node($1->first, false);
    if (node == NULL)
      node = new SSA_Node(SSA_AssignNode, $1->first);

    SSA_Stmt *stmt = new SSA_Stmt(SSA_AssignStmt, "=", $4, $6, NULL);

    SSA_Meta *meta = new SSA_Meta(*$1, new list<SSA_Stmt *>({$3, stmt}));

    node->add_meta(meta, $1->second);
    program->ssa_nodes->insert(make_pair($1->first, node));

    $$ = node;
  }
;

PhiStmt
  : Var SSA_ASSIGN SSA_PHI SSA_LRB VarList SSA_RRB SSA_EOS
  {
    SSA_Stmt *stmt = new SSA_Stmt(SSA_PhiStmt, $1, $5);

    $$ = stmt;
  }
;

VarList
  : Var
  {
    $$ = new list<SSA_Opd *>();
    $$->push_back($1);
  }
  | VarList SSA_COMMA Var
  {
    $$ = $1;
    $$->push_back($3);
  }
;

/* Edges */

Edge
  : SSA_NUM SSA_ARROW SSA_NUM SSA_EOS
  {
    SSA_Edge *edge = new SSA_Edge($1, $3);

    edge->get_src()->add_out_edge(edge, $3);
    edge->get_dst()->add_in_edge(edge, $1);
    program->ssa_edges->insert(make_pair(make_pair($1, $3), edge));

    $$ = edge;
  }
;

/* Operand */

Opd
  : SSA_NUM
  {
    $$ = new SSA_Opd(SSA_NumOpd, $1);
  }
  | Var
  {
    $$ = $1;
  }
;

Var
  : SSA_ID SSA_UNDERSCORE MetaNum
  {
    $$ = new SSA_Opd(SSA_VarOpd, *$3, *$1);
  }
;

MetaNum
  : SSA_NUM SSA_UNDERSCORE SSA_NUM
  {
    $$ = new pair<int, int>(make_pair($1, $3));
  }
;

%%

int ssa_error(const char *msg) {
  cerr << "Syntax Error: Line " << ssa_lineno << ": " << msg << "\n";
  cleanup();
  return 1;
}
