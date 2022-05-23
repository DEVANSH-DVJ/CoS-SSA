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
  | ProcDefList SSA_COMMA SSA_ID
;

ProcList
  : Proc
  | ProcList Proc
;

/* Procedure */

Proc
  : SSA_LCB StartNode NodeList EndNode EdgeList SSA_RCB
;

NodeList
  : Node
  | NodeList Node
;

EdgeList
  : Edge
  | EdgeList Edge
;

/* Statements */

StartNode
  : MetaNum SSA_COLON SSA_START SSA_ID SSA_EOS
;

EndNode
  : MetaNum SSA_COLON SSA_END SSA_ID SSA_EOS
;

Node
  : CallNode
  | InputNode
  | UsevarNode
  | ExprNode
;

CallNode
  : MetaNum SSA_COLON SSA_CALL SSA_ID SSA_EOS
;

InputNode
  : MetaNum SSA_COLON Var SSA_ASSIGN SSA_INPUT SSA_EOS
;

UsevarNode
  : MetaNum SSA_COLON SSA_USEVAR SSA_ASSIGN Var SSA_EOS
  | MetaNum SSA_COLON PhiStmt SSA_USEVAR SSA_ASSIGN Var SSA_EOS
;

ExprNode
  : MetaNum SSA_COLON Var SSA_ASSIGN Opd SSA_OP Opd SSA_EOS
  | MetaNum SSA_COLON PhiStmt Var SSA_ASSIGN Opd SSA_OP Opd SSA_EOS
  | MetaNum SSA_COLON PhiStmt PhiStmt Var SSA_ASSIGN Opd SSA_OP Opd SSA_EOS
  | MetaNum SSA_COLON Var SSA_ASSIGN Opd SSA_EOS
  | MetaNum SSA_COLON PhiStmt Var SSA_ASSIGN Opd SSA_EOS
;

PhiStmt
  : Var SSA_ASSIGN SSA_PHI SSA_LRB VarList SSA_RRB SSA_EOS
;

VarList
  : Var
  | VarList SSA_COMMA Var
;

/* Edges */

Edge
  : SSA_NUM SSA_ARROW SSA_NUM SSA_EOS
;

/* Operand */

Opd
  : SSA_NUM
  | Var
;

Var
  : SSA_ID SSA_UNDERSCORE MetaNum
;

MetaNum
  : SSA_NUM SSA_UNDERSCORE SSA_NUM
;

%%

int ssa_error(const char *msg) {
  cerr << "Syntax Error: Line " << ssa_lineno << ": " << msg << "\n";
  cleanup();
  return 1;
}
