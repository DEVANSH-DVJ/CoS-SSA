%{
#include "common-headers.hh"
#include "parse.tab.h"

%}
%option noyywrap

digit  [0-9]
ws	[ \t\n]*
operator [-+*/]
letter [a-zA-Z_]

%%
"//".*			    {  ; }
{digit}+ 		    { 	yylval.num= atoi(yytext); return NUM; }
"->"			    {	return ARROW;	}
{operator}		    { 	yylval.name= new string(yytext); return OP;	}
"="|";"|":"|","		    {	return yytext[0]; }
"call"|"Call"		    {	return CALL;	}
"start"|"Start"		    {	return START;	}
"end"|"End"		    {	return END;	}
{letter}({letter}|{digit})* {   yylval.name= new string(yytext); return ID; }
		
{ws}		;
.	{ 	
		string error_message;
		error_message =  "Illegal character `" + *yytext;
		error_message += "' on line " + yylineno;
	}

