%{

    #include <string>
    using namespace std;

    #include "cfg.tab.h"

%}

%option noyywrap
%option prefix="cfg_"

digit [0-9]
letter [a-zA-Z]
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
{operator} { cfg_lval.name = new string(cfg_text); return OP; }

  /* Identifiers */

{letter}+ { cfg_lval.name = new string(cfg_text); return ID; }

  /* Constants */

{digit}+ { cfg_lval.value = atoi(cfg_text); return NUM; }

  /* Rest */

. { return UNKNOWN; }
