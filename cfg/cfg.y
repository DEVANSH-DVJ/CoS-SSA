%{

  #include <iostream>
  #include <string>

  using namespace std;

  #include "../headers.hh"

  extern int cfg_lex(void);

  extern int cfg_lineno;
  extern int cfg_error(const char *);

%}
%union{
  string *name;
  int value;
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

%left '+' '-'
%left '*' '/'

%%

/* Program */
Program
  : ProcDefList CFG_EOS ProcList
;

ProcDefList
  : CFG_ID
  | ProcDefList CFG_COMMA CFG_ID
;

ProcList
  : Proc
  | ProcList Proc
;

/* Procedure */

Proc
  : CFG_LCB StartNode NodeList EndNode EdgeList CFG_RCB
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
  : CFG_NUM CFG_COLON CFG_START CFG_ID CFG_EOS
;

EndNode
  : CFG_NUM CFG_COLON CFG_END CFG_ID CFG_EOS
;

Node
  : CallNode
  | InputNode
  | UsevarNode
  | ExprNode
;

CallNode
  : CFG_NUM CFG_COLON CFG_CALL CFG_ID CFG_EOS
;

InputNode
  : CFG_NUM CFG_COLON CFG_ID CFG_ASSIGN CFG_INPUT CFG_EOS
;

UsevarNode
  : CFG_NUM CFG_COLON CFG_USEVAR CFG_ASSIGN CFG_ID CFG_EOS
;

ExprNode
  : CFG_NUM CFG_COLON CFG_ID CFG_ASSIGN Opd CFG_OP Opd CFG_EOS
  | CFG_NUM CFG_COLON CFG_ID CFG_ASSIGN Opd CFG_EOS
;

/* Edges */

Edge
  : CFG_NUM CFG_ARROW CFG_NUM CFG_EOS
;

/* Operand */

Opd
  : CFG_NUM
  | CFG_ID
;

%%

int yyerror(const char *msg) {
  cerr << "Syntax Error: Line " << cfg_lineno << ": " << msg << "\n";
  cleanup();
  return 1;
}
