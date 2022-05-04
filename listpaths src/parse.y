%{
#include "common-headers.hh"
extern "C" void yyerror(char *s);
extern int yylex(void);

Procedure * current_procedure;

Program program;

%}
%union{
	string * name;
	int num;
	Opd * opd;
}
%token <num> NUM 
%token <name> ID
%token <name> OP
%type <opd> Opd
%token CALL  START  END  ARROW 
%left '+' '-'
%left '*' '/'

%%

Program : ProcList
	;

ProcList : Proc
	 | ProcList Proc
	;

Proc : START ID 
     		{
			current_procedure = new Procedure(*$2);
		}
     	StmtList 
	EdgeList END 
     		{
			program.add_procedure(current_procedure);
		}

	;

StmtList : Stmt 			
	| StmtList Stmt 		
	;

Stmt : NUM ':' ID '=' Opd OP Opd   		
     				{  
					Node * n = new Node($1,*$3,*$5,*$6,*$7);
					current_procedure->add_node(n);					
				}
	| NUM ':' ID '=' Opd  		
     				{  
					Node * n = new Node($1,*$3,*$5);
					current_procedure->add_node(n);					
				}
	| NUM ':' CALL ID
     				{  
					Node * n = new Node($1,*$4);
					current_procedure->add_node(n);					
				}

Opd : NUM
    				{
					$$ = new Opd($1);
				}
    | ID
    				{
					$$ = new Opd(*$1);
				}
	;

	;
EdgeList :  Edge
	 | EdgeList ',' Edge
	;

Edge : NUM ARROW NUM 
	 			{
					current_procedure->add_edge($1, $3);					

				}

	
%%
