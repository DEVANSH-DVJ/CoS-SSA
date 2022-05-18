#include <fstream>
#include <iomanip>
#include <iostream>

#include <stdlib.h>
#include <string>

using namespace std;

#include "argparse.hh"
#include "cfg/cfg.tab.h"
#include "ssa/ssa.tab.h"

#include "headers.hh"

extern int cfg_lex(void);
extern void cfg_set_in(FILE *);
extern void cfg_set_out(FILE *);
extern FILE *cfg_out;

extern int ssa_lex(void);
extern void ssa_set_in(FILE *);
extern void ssa_set_out(FILE *);
extern FILE *ssa_out;

bool viz;

string tokens_file = "/dev/null";
string dot_file = "/dev/null";
string png_file = "/dev/null";

fstream *dot_fd;

int main(int argc, char **argv) {

  // Default arguments
  struct arguments arguments;

  arguments.visualize = 0;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

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
  ssa_set_in(in_file);
  ssa_set_out(out_file);

  dot_fd = new fstream(dot_file.c_str(), ios::out | ios::trunc);

  if (arguments.visualize) {
    viz = true;
  } else {
    viz = false;
  }

  ssa_parse();

  fclose(ssa_out);

  exit(0);
}
