%{

  #include <iostream>
  #include <string>

  #include "program.hh"
  #include "procedure.hh"
  #include "node.hh"
  #include "edge.hh"

  using namespace std;

  extern int yylex(void);

  extern int yylineno;
  extern int yyerror(const char *);
  extern void cleanup(const char *);

  extern bool viz;

%}
%union{
  string *name;
  int value;
  Program *program;
  list<Procedure *> *procedures;
  Procedure *procedure;
  list<Node *> *nodes;
  Node *node;
  list<Edge *> *edges;
  Edge *edge;
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
%type <edges> EdgeList
%type <node> StartStmt
%type <node> EndStmt
%type <node> Stmt
%type <edges> Edges
%type <edge> Edge
%type <opd> Opd

%%

/* Program */
Program
  : ProcList
  {
    $$ = new Program($1);
    $$->finish(viz);
  }
;

ProcList
  : Proc
  {
    $$ = new list<Procedure *>();
    $$->push_back($1);
  }
  | ProcList Proc
  {
    $$ = $1;
    $$->push_back($2);
  }
;

/* Procedure */

Proc
  : StartStmt StmtList EndStmt EdgeList
  {
    $$ = new Procedure();
    $$->nodes->push_back($1);
    $$->nodes->splice($$->nodes->end(), *$2);
    $$->nodes->push_back($3);
    $$->edges->splice($$->edges->end(), *$4);
  }
;

StmtList
  : Stmt
  {
    $$ = new list<Node *>();
    $$->push_back($1);
  }
  | StmtList Stmt
  {
    $$ = $1;
    $$->push_back($2);
  }
;

EdgeList
  : EOS
  {
    $$ = new list<Edge *>();
  }
  | Edges EOS
  {
    $$ = $1;
  }
;

/* Statements */

StartStmt
  : NUM COLON START ID
  {
    $$ = new TerminalNode($1, start_stmt, *$4);
  }
;

EndStmt
  : NUM COLON END ID
  {
    $$ = new TerminalNode($1, end_stmt, *$4);
  }
;

Stmt
  : NUM COLON ID ASSIGN Opd OP Opd
  {
    $$ = new ExprNode($1, *$6, new VarOpd(*$3), $5, $7);
  }
  | NUM COLON ID ASSIGN Opd
  {
    $$ = new CopyNode($1, new VarOpd(*$3), $5);
  }
  | NUM COLON CALL ID
  {
    $$ = new CallNode($1, *$4);
  }
;

/* Edges */

Edges
  : Edge
  {
    $$ = new list<Edge *>();
    $$->push_back($1);
  }
  | Edges COMMA Edge
  {
    $$ = $1;
    $$->push_back($3);
  }
;

Edge
  : NUM ARROW NUM
  {
    $$ = new Edge($1, $3);
  }
;

/* Operand */

Opd
  : NUM
  {
    $$ = new IntOpd($1);
  }
  | ID
  {
    $$ = new VarOpd(*$1);
  }
;

%%

int yyerror(const char *msg) {
  cerr << "Error in line " << yylineno << ": " << msg;
  cleanup("");
  return 1;
}
