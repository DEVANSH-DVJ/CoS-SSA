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
#include "edge.hh"
#include "y.tab.h"

extern int yylex(void);
extern void yyset_in(FILE *);
extern void yyset_out(FILE *);
extern FILE *yyout;

bool viz;

string tokens_file = "/dev/null";
string gv_file = "/dev/null";

fstream *gv_fd;

void cleanup(const char *msg) {
  cerr << msg << endl;

  yyout = freopen(NULL, "w", yyout);

  gv_fd->close();
  gv_fd->open(gv_file.c_str(), ios::out | ios::trunc);

  fclose(yyout);
  gv_fd->close();

  exit(1);
}

int main(int argc, char **argv) {

  // Default arguments
  struct arguments arguments;

  arguments.visualize = 0;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  char *input_file = (char *)arguments.input_file.c_str();

  if (arguments.visualize)
    gv_file = arguments.input_file + ".gv";

  FILE *in_file = fopen(input_file, "r");
  if (in_file == NULL) {
    printf("Error opening file: %s\n", input_file);
    exit(1);
  }

  FILE *out_file = fopen(tokens_file.c_str(), "w");
  yyset_in(in_file);
  yyset_out(out_file);

  gv_fd = new fstream(gv_file.c_str(), ios::out | ios::trunc);

  if (arguments.visualize) {
    viz = true;
    cout << "Visualizing..." << endl;
  } else {
    viz = false;
  }

  yyparse();

  fclose(yyout);

  exit(0);
}
