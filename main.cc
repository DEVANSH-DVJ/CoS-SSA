#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdlib.h>

#include "argparse.hh"

#include <list>
#include <string>

#include "headers.hh"

using namespace std;

Program *program;

fstream *dot_fd;

int main(int argc, char **argv) {

  // Default arguments
  struct arguments arguments;

  arguments.input_type = FILE_UNKNOWN;
  arguments.input_file = "";

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  exit(0);
}
