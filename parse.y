%{

  #include <iostream>
  #include <string>

  using namespace std;

  extern int yylex(void);

  extern int yylineno;
  extern int yyerror(const char *);
  extern void cleanup(const char *);

%}
%union{
  string *name;
  int value;
}

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
  : ProcList
;

ProcList
  : Proc
  | ProcList Proc
;

/* Procedure */

Proc
  : StartStmt StmtList EndStmt EdgeList
;

StartStmt
  : NUM COLON START ID
;

EndStmt
  : NUM COLON END
;

EdgeList
  : EOS
  | Edges EOS
;

StmtList
  : Stmt
  | StmtList Stmt
;

/* Statements */

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
  cerr << "Error in line " << yylineno << ": " << msg;
  cleanup("");
  return 1;
}
