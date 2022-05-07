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
  {
    $$ = new Program($1);
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
    string proc_name = ((TerminalNode *)$1)->name;
    $$ = new Procedure(proc_name);
    ((TerminalNode *)$3)->name = proc_name;
    $$->stmts->push_back($1);
    $$->stmts->splice($$->stmts->end(), *$2);
    $$->stmts->push_back($3);
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
  | Edges EOS
;

/* Statements */

StartStmt
  : NUM COLON START ID
  {
    $$ = new TerminalNode($1, start_stmt, *$4);
  }
;

EndStmt
  : NUM COLON END
  {
    $$ = new TerminalNode($1, end_stmt, "");
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
  | Edges COMMA Edge
;

Edge
  : NUM ARROW NUM
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
