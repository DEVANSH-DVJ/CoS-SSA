#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdlib.h>

#include "argparse.hh"

#include <list>
#include <string>

#include "headers.hh"

using namespace std;

#include "cfg/cfg.tab.hh"
#include "ssa/ssa.tab.hh"

extern int cfg_lex(void);
extern void cfg_set_in(FILE *);
extern void cfg_set_out(FILE *);
extern FILE *cfg_out;

extern int ssa_lex(void);
extern void ssa_set_in(FILE *);
extern void ssa_set_out(FILE *);
extern FILE *ssa_out;

Program *program;

string cfg_file = "/dev/null";
string ssa_file = "/dev/null";
string dot_file = "/dev/null";
string png_file = "/dev/null";

fstream *dot_fd;

int main(int argc, char **argv) {

  // Default arguments
  struct arguments arguments;

  arguments.visualize = 0;
  arguments.input_type = FILE_UNKNOWN;
  arguments.input_file = "";

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  program = new Program();

  char *input_file = (char *)arguments.input_file.c_str();

  dot_file = arguments.input_file + ".dot";
  png_file = arguments.input_file + ".png";

  FILE *in_file = fopen(input_file, "r");
  if (in_file == NULL) {
    printf("Error opening file: %s\n", input_file);
    exit(1);
  }

  if (arguments.input_type == FILE_CFG) {
    cfg_set_in(in_file);
  } else if (arguments.input_type == FILE_SSA) {
    ssa_set_in(in_file);
  } else {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Unknown input type");
  }

  cfg_set_out(fopen(cfg_file.c_str(), "w"));
  ssa_set_out(fopen(ssa_file.c_str(), "w"));

  dot_fd = new fstream(dot_file.c_str(), ios::out | ios::trunc);

  if (arguments.input_type == FILE_CFG) {
    cfg_parse();
    cout << "CFG nodes: " << program->cfg_nodes->size() << endl;
    cout << "CFG edges: " << program->cfg_edges->size() << endl;
    for (map<string, Procedure *>::iterator it = program->procedures->begin();
         it != program->procedures->end(); ++it) {
      cout << "Procedure " << it->first << ":" << endl;
      cout << "CFG nodes: " << it->second->cfg_nodes->size() << endl;
      cout << "CFG edges: " << it->second->cfg_edges->size() << endl;
    }
    program->visualize_cfg();

  } else if (arguments.input_type == FILE_SSA) {
    ssa_parse();
  }

  if (arguments.input_type == FILE_CFG) {
    fclose(cfg_out);
  } else if (arguments.input_type == FILE_SSA) {
    fclose(ssa_out);
  }

  dot_fd->close();
  system(("dot -Tpng " + dot_file + " -o " + png_file).c_str());

  exit(0);
}
