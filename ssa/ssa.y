%{

  #include <iostream>

  using namespace std;

  extern int ssa_lex(void);

  extern int ssa_lineno;
  extern int ssa_error(const char *);
  extern void cleanup();

%}
%union{
  string *name;
  int value;
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
  : SSA_NUM SSA_COLON SSA_START SSA_ID SSA_EOS
;

EndNode
  : SSA_NUM SSA_COLON SSA_END SSA_ID SSA_EOS
;

Node
  : InputNode
  | CallNode
  | UseNode
  | DepNode
;

CallNode
  : SSA_NUM SSA_COLON SSA_CALL SSA_ID SSA_EOS
;

InputNode
  : SSA_NUM SSA_COLON Var SSA_ASSIGN SSA_INPUT SSA_EOS
;

UseNode
  : SSA_NUM SSA_COLON SSA_USEVAR SSA_ASSIGN Var SSA_EOS
  | SSA_NUM SSA_COLON PhiStmt SSA_USEVAR SSA_ASSIGN Var SSA_EOS
;

DepNode
  : SSA_NUM SSA_COLON Var SSA_ASSIGN Opd SSA_OP Opd SSA_EOS
  | SSA_NUM SSA_COLON PhiStmt Var SSA_ASSIGN Opd SSA_OP Opd SSA_EOS
  | SSA_NUM SSA_COLON PhiStmt PhiStmt Var SSA_ASSIGN Opd SSA_OP Opd SSA_EOS
  | SSA_NUM SSA_COLON Var SSA_ASSIGN Opd SSA_EOS
  | SSA_NUM SSA_COLON PhiStmt Var SSA_ASSIGN Opd SSA_EOS
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
  : SSA_ID SSA_UNDERSCORE SSA_NUM SSA_UNDERSCORE SSA_NUM
;

%%

int yyerror(const char *msg) {
  cerr << "Syntax Error: Line " << ssa_lineno << ": " << msg << "\n";
  cleanup();
  return 1;
}
