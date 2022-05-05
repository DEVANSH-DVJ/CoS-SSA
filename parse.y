%{

  #include <iostream>
  #include <string>

  using namespace std;

  extern int yylex(void);

  extern FILE *yyout;
  extern char *yytext;
  extern int yylineno;
  extern int yyerror(const char *);
  extern void cleanup(const char *);

%}
%union{
  string *name;
  int num;
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
%token <num> NUM
%token UNKNOWN

%left '+' '-'
%left '*' '/'

%%

Program
  : ProcList
;

ProcList
  : Proc
  | ProcList Proc
;

Proc
  : NUM COLON START ID
  StmtList
  NUM COLON END
  EdgeList EOS
;

StmtList
  : Stmt
  | StmtList Stmt
;

Stmt
  : NUM COLON ID ASSIGN Opd OP Opd
  | NUM COLON ID ASSIGN Opd
  | NUM COLON CALL ID
;

Opd
  : NUM
  | ID
;

EdgeList
  : /*EMPTY*/
  | Edges
;

Edges
  : Edge
  | Edges COMMA Edge
;

Edge
  : NUM ARROW NUM
;

%%

int yyerror(const char *msg) {
  cerr << "Error in line " << yylineno << ": " << msg;
  cleanup("");
  return 1;
}
