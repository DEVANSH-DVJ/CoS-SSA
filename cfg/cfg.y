%{

  #include <iostream>

  using namespace std;

  extern int cfg_lex(void);

  extern int cfg_lineno;
  extern int cfg_error(const char *);
  extern void cleanup();

%}
%union{
  string *name;
  int value;
}

%define api.prefix {cfg_}

%token EOS
%token COLON
%token ARROW
%token COMMA
%token LBRACE
%token RBRACE

%token START
%token END
%token CALL
%token INPUT
%token USEVAR

%token ASSIGN
%token <name> OP

%token <name> ID
%token <value> NUM

%token UNKNOWN

%left '+' '-'
%left '*' '/'

%%

/* Program */
Program
  : ProcDefList EOS ProcList
;

ProcDefList
  : ID
  | ProcDefList COMMA ID
;

ProcList
  : Proc
  | ProcList Proc
;

/* Procedure */

Proc
  : LBRACE StartNode NodeList EndNode EdgeList RBRACE
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
  : NUM COLON START ID EOS
;

EndNode
  : NUM COLON END ID EOS
;

Node
  : InputNode
  | CallNode
  | UseNode
  | DepNode
;

CallNode
  : NUM COLON CALL ID EOS
;

InputNode
  : NUM COLON ID ASSIGN INPUT EOS
;

UseNode
  : NUM COLON USEVAR ASSIGN ID EOS
;

DepNode
  : NUM COLON ID ASSIGN Opd OP Opd EOS
  | NUM COLON ID ASSIGN Opd EOS
;

/* Edges */

Edge
  : NUM ARROW NUM EOS
;

/* Operand */

Opd
  : NUM
  | ID
;

%%

int yyerror(const char *msg) {
  cerr << "Syntax Error: Line " << cfg_lineno << ": " << msg << "\n";
  cleanup();
  return 1;
}
