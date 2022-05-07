#include <fstream>
#include <iomanip>
#include <iostream>

#include <stdlib.h>
#include <string>

using namespace std;

#include "argparse.hh"

#include "program.hh"
#include "procedure.hh"
#include "node.hh"
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

  // Default arguments
  struct arguments arguments;

  arguments.visualize = 0;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  char *input_file = (char *)arguments.input_file.c_str();

  FILE *in_file = fopen(input_file, "r");
  if (in_file == NULL) {
    printf("Error opening file: %s\n", input_file);
    exit(1);
  }

  FILE *out_file = fopen("/dev/null", "w");
  yyset_in(in_file);
  yyset_out(out_file);

  yyparse();

  if (arguments.visualize) {
    cout << "Visualizing..." << endl;
  }

  fclose(yyout);

  exit(0);
}
