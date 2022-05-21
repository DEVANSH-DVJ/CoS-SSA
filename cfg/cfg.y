%{

  #include <iostream>
  #include <string>

  using namespace std;

  #include "../headers.hh"

  extern int cfg_lex(void);

  extern int cfg_lineno;
  extern int cfg_error(const char *);

  extern Program *program;

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
  {
    program->procedures->insert(make_pair(*$1, new Procedure(*$1)));
  }
  | ProcDefList CFG_COMMA CFG_ID
  {
    program->procedures->insert(make_pair(*$3, new Procedure(*$3)));
  }
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
  {
    CFG_Node *node = new CFG_Node(CFG_Start, $1);
    program->cfg_nodes->insert(make_pair($1, node));
  }
;

EndNode
  : CFG_NUM CFG_COLON CFG_END CFG_ID CFG_EOS
  {
    CFG_Node *node = new CFG_Node(CFG_End, $1);
    program->cfg_nodes->insert(make_pair($1, node));
  }
;

Node
  : CallNode
  | InputNode
  | UsevarNode
  | ExprNode
;

CallNode
  : CFG_NUM CFG_COLON CFG_CALL CFG_ID CFG_EOS
  {
    CFG_Node *node = new CFG_Node(CFG_Call, $1);
    program->cfg_nodes->insert(make_pair($1, node));
  }
;

InputNode
  : CFG_NUM CFG_COLON CFG_ID CFG_ASSIGN CFG_INPUT CFG_EOS
  {
    CFG_Node *node = new CFG_Node(CFG_Input, $1);
    program->cfg_nodes->insert(make_pair($1, node));
  }
;

UsevarNode
  : CFG_NUM CFG_COLON CFG_USEVAR CFG_ASSIGN CFG_ID CFG_EOS
  {
    CFG_Node *node = new CFG_Node(CFG_Usevar, $1);
    program->cfg_nodes->insert(make_pair($1, node));
  }
;

ExprNode
  : CFG_NUM CFG_COLON CFG_ID CFG_ASSIGN Opd CFG_OP Opd CFG_EOS
  {
    CFG_Node *node = new CFG_Node(CFG_Expr, $1);
    program->cfg_nodes->insert(make_pair($1, node));
  }
  | CFG_NUM CFG_COLON CFG_ID CFG_ASSIGN Opd CFG_EOS
  {
    CFG_Node *node = new CFG_Node(CFG_Expr, $1);
    program->cfg_nodes->insert(make_pair($1, node));
  }
;

/* Edges */

Edge
  : CFG_NUM CFG_ARROW CFG_NUM CFG_EOS
  {
    CFG_Node *from = program->cfg_nodes->find($1)->second;
    CFG_Node *to = program->cfg_nodes->find($3)->second;
    CFG_Edge *edge = new CFG_Edge(from, to);
    from->out_edges->insert(make_pair($3, edge));
    to->in_edges->insert(make_pair($1, edge));
    program->cfg_edges->insert(make_pair(make_pair($1, $3), edge));
  }
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
