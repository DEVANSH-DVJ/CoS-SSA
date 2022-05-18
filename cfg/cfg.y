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

%token CALL
%token START
%token END

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
  : ProcDefList ProcList
;

ProcDefList
  : ID
  | ProcDefList ID
;

ProcList
  : Proc
  | ProcList Proc
;

/* Procedure */

Proc
  : StartStmt StmtList EndStmt EdgeList
;

StmtList
  : Stmt
  | StmtList Stmt
;

EdgeList
  : EOS
  | Edges EOS
;

/* Statements */

StartStmt
  : NUM COLON START ID
;

EndStmt
  : NUM COLON END ID
;

Stmt
  : NUM COLON ID ASSIGN Opd OP Opd
  | NUM COLON ID ASSIGN Opd
  | NUM COLON CALL ID
;

/* Edges */

Edges
  : Edge
  | Edges COMMA Edge
;

Edge
  : NUM ARROW NUM
;

/* Operand */

Opd
  : NUM
  | ID
;

%%

int yyerror(const char *msg) {
  cerr << "Syntax Error: Line " << cfg_lineno << ": " << msg;
  cleanup();
  return 1;
}
