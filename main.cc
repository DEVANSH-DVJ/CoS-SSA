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
string dot_file = "/dev/null";
string png_file = "/dev/null";

fstream *dot_fd;

void cleanup(const char *msg) {
  cerr << msg << endl;

  yyout = freopen(NULL, "w", yyout);

  dot_fd->close();
  dot_fd->open(dot_file.c_str(), ios::out | ios::trunc);

  fclose(yyout);
  dot_fd->close();

  exit(1);
}

int main(int argc, char **ardot) {

  // Default arguments
  struct arguments arguments;

  arguments.visualize = 0;

  argp_parse(&argp, argc, ardot, 0, 0, &arguments);

  char *input_file = (char *)arguments.input_file.c_str();

  if (arguments.visualize) {
    dot_file = arguments.input_file + ".dot";
    png_file = arguments.input_file + ".png";
  }

  FILE *in_file = fopen(input_file, "r");
  if (in_file == NULL) {
    printf("Error opening file: %s\n", input_file);
    exit(1);
  }

  FILE *out_file = fopen(tokens_file.c_str(), "w");
  yyset_in(in_file);
  yyset_out(out_file);

  dot_fd = new fstream(dot_file.c_str(), ios::out | ios::trunc);

  if (arguments.visualize) {
    viz = true;
  } else {
    viz = false;
  }

  yyparse();

  fclose(yyout);

  exit(0);
}
