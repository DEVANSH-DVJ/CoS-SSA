%{

  #include <iostream>
  #include <string>

  #include "program.hh"
  #include "procedure.hh"
  #include "node.hh"

  using namespace std;

  extern int yylex(void);

  extern int yylineno;
  extern int yyerror(const char *);
  extern void cleanup(const char *);

%}
%union{
  string *name;
  int value;
  Program *program;
  list<Procedure *> *procedures;
  Procedure *procedure;
  list<Node *> *nodes;
  Node *node;
  Opd *opd;
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

%type <program> Program
%type <procedures> ProcList
%type <procedure> Proc
%type <nodes> StmtList
%type <node> StartStmt
%type <node> EndStmt
%type <node> Stmt
%type <opd> Opd

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
