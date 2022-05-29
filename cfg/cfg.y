%{

  #include <iostream>
  #include <string>

  #include "../headers.hh"

  using namespace std;

  extern int cfg_lex(void);

  extern int cfg_lineno;
  extern int cfg_error(const char *);

  extern Program *program;

%}
%union{
  string *name;
  int value;

  Procedure *proc;

  list<CFG_Node *> *cfg_node_list;
  list<CFG_Edge *> *cfg_edge_list;

  CFG_Node *cfg_node;
  CFG_Edge *cfg_edge;
  CFG_Opd *opd;
}

%define api.prefix {cfg_}

%token CFG_EOS
%token CFG_COLON
%token CFG_ARROW
%token CFG_COMMA
%token CFG_LCB
%token CFG_RCB

%token CFG_START
%token CFG_END
%token CFG_CALL
%token CFG_INPUT
%token CFG_USEVAR

%token CFG_ASSIGN
%token <name> CFG_OP

%token <name> CFG_ID
%token <value> CFG_NUM

%token CFG_UNKNOWN

%type <proc> Proc

%type <cfg_node_list> NodeList
%type <cfg_edge_list> EdgeList

%type <cfg_node> StartNode
%type <cfg_node> EndNode
%type <cfg_node> Node
%type <cfg_node> CallNode
%type <cfg_node> InputNode
%type <cfg_node> UsevarNode
%type <cfg_node> ExprNode

%type <cfg_edge> Edge

%type <opd> Opd

%left '+' '-'
%left '*' '/'

%%

/* Program */
Program
  : ProcDefList CFG_EOS ProcList
;

ProcDefList
  : CFG_ID
  {
    program->procedures->insert(make_pair(*$1, new Procedure(*$1)));
  }
  | ProcDefList CFG_COMMA CFG_ID
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
  : CFG_LCB StartNode NodeList EndNode EdgeList CFG_RCB
  {
    string proc_name = $2->get_parent_proc();
    CHECK_INPUT_AND_ABORT(proc_name == $4->get_parent_proc(),
                          "Start node and end node must be in the same procedure");

    Procedure *proc = program->get_proc(proc_name);
    proc->cfg_nodes->insert(make_pair($2->get_node_id(), $2));
    proc->cfg_nodes->insert(make_pair($4->get_node_id(), $4));
    for (list<CFG_Node *>::iterator it = $3->begin(); it != $3->end(); ++it) {
      proc->cfg_nodes->insert(make_pair((*it)->get_node_id(), *it));
      (*it)->set_parent_proc(proc_name);
    }
    for (list<CFG_Edge *>::iterator it = $5->begin(); it != $5->end(); ++it)
      proc->cfg_edges->insert(make_pair((*it)->get_edge_id(), *it));

    $$ = proc;
  }
;

NodeList
  : Node
  {
    $$ = new list<CFG_Node *>();
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
    $$ = new list<CFG_Edge *>();
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
  : CFG_NUM CFG_COLON CFG_START CFG_ID CFG_EOS
  {
    string stmt = "START " + *$4;
    CFG_Node *node = new CFG_Node(CFG_StartNode, $1, stmt);
    node->set_parent_proc(*$4);

    program->cfg_nodes->insert(make_pair($1, node));

    $$ = node;
  }
;

EndNode
  : CFG_NUM CFG_COLON CFG_END CFG_ID CFG_EOS
  {
    string stmt = "END " + *$4;
    CFG_Node *node = new CFG_Node(CFG_EndNode, $1, stmt);
    node->set_parent_proc(*$4);

    program->cfg_nodes->insert(make_pair($1, node));

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
  : CFG_NUM CFG_COLON CFG_CALL CFG_ID CFG_EOS
  {
    string stmt = "CALL " + *$4;
    CFG_Node *node = new CFG_Node(CFG_CallNode, $1, stmt,
                                  *$4);

    program->cfg_nodes->insert(make_pair($1, node));

    $$ = node;
  }
;

InputNode
  : CFG_NUM CFG_COLON CFG_ID CFG_ASSIGN CFG_INPUT CFG_EOS
  {
    string stmt = *$3 + " = INPUT";
    CFG_Node *node = new CFG_Node(CFG_AssignNode, $1, stmt,
                                  "=",
                                  new CFG_Opd(CFG_VarOpd, *$3),
                                  new CFG_Opd(CFG_InputOpd),
                                  NULL);

    program->cfg_nodes->insert(make_pair($1, node));

    $$ = node;
  }
;

UsevarNode
  : CFG_NUM CFG_COLON CFG_USEVAR CFG_ASSIGN CFG_ID CFG_EOS
  {
    string stmt = "USEVAR = " + *$5;
    CFG_Node *node = new CFG_Node(CFG_AssignNode, $1, stmt,
                                  "=",
                                  new CFG_Opd(CFG_UsevarOpd),
                                  new CFG_Opd(CFG_VarOpd, *$5),
                                  NULL);

    program->cfg_nodes->insert(make_pair($1, node));

    $$ = node;
  }
;

ExprNode
  : CFG_NUM CFG_COLON CFG_ID CFG_ASSIGN Opd CFG_OP Opd CFG_EOS
  {
    string stmt = *$3 + " = " + $5->str() + " " + *$6 + " " + $7->str();
    CFG_Node *node = new CFG_Node(CFG_AssignNode, $1, stmt,
                                  *$6,
                                  new CFG_Opd(CFG_VarOpd, *$3),
                                  $5,
                                  $7);

    program->cfg_nodes->insert(make_pair($1, node));

    $$ = node;
  }
  | CFG_NUM CFG_COLON CFG_ID CFG_ASSIGN Opd CFG_EOS
  {
    string stmt = *$3 + " = " + $5->str();
    CFG_Node *node = new CFG_Node(CFG_AssignNode, $1, stmt,
                                  "=",
                                  new CFG_Opd(CFG_VarOpd, *$3),
                                  $5,
                                  NULL);

    program->cfg_nodes->insert(make_pair($1, node));

    $$ = node;
  }
;

/* Edges */

Edge
  : CFG_NUM CFG_ARROW CFG_NUM CFG_EOS
  {
    CFG_Edge *edge = new CFG_Edge($1, $3);

    edge->get_src()->add_out_edge(edge);
    edge->get_dst()->add_in_edge(edge);
    program->cfg_edges->insert(make_pair(make_pair($1, $3), edge));

    $$ = edge;
  }
;

/* Operand */

Opd
  : CFG_NUM
  {
    $$ = new CFG_Opd(CFG_NumOpd, $1);
  }
  | CFG_ID
  {
    $$ = new CFG_Opd(CFG_VarOpd, *$1);
  }
;

%%

int cfg_error(const char *msg) {
  cerr << "Syntax Error: Line " << cfg_lineno << ": " << msg << "\n";
  cleanup();
  return 1;
}
