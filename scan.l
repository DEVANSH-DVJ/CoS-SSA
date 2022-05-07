%{

    using namespace std;

    #include <string>

    #include "y.tab.h"

%}

%option noyywrap

digit [0-9]
letter [a-zA-Z_]
operator [-+*/]
ignore [ \t\n]

%%

  /* Ignore characters */

{ignore}* ;
"//".* ;

  /* Delimiters */

";" { return EOS; }
":" { return COLON; }
"->" { return ARROW; }
"," { return COMMA; }

  /* Keywords */

"call"|"Call" { return CALL; }
"start"|"Start" { return START; }
"end"|"End" { return END; }

  /* Identifiers */

"=" { return ASSIGN; }
{operator} { yylval.name = new string(yytext); return OP; }

  /* Identifiers */

{letter}({letter}|{digit})* { yylval.name = new string(yytext); return ID; }

  /* Constants */

{digit}+ { yylval.value = atoi(yytext); return NUM; }

  /* Rest */

. { return UNKNOWN; }
