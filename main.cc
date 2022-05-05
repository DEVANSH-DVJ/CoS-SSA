#include <fstream>
#include <iomanip>
#include <iostream>

#include <stdlib.h>
#include <string>

using namespace std;

#include "y.tab.h"

extern int yylex(void);
extern void yyset_in(FILE *);
extern void yyset_out(FILE *);
extern FILE *yyout;

void cleanup(const char *msg) {
  cerr << msg << endl;

  yyout = freopen(NULL, "w", yyout);
  fclose(yyout);

  exit(1);
}

int main(int argc, char **argv) {
  yyparse();
  fclose(yyout);

  exit(0);
}
