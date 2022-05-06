#ifndef _ARGPARSE_HH_
#define _ARGPARSE_HH_

#include <argp.h>
#include <string>

using namespace std;

enum short_options {
  OPT_VISUALIZE = 0x80,
};

struct arguments {
  int visualize;
  string input_file;
};

/* Program documentation */
char doc[] = "CS-SSA";

/* Argument description */
char args_doc[] = "[FILE]";

/* Options visible to user */
struct argp_option options[] = {{"visualize", OPT_VISUALIZE, 0, 0, "Visualize"},
                                {0}};

error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = (struct arguments *)state->input;

  switch (key) {
  case OPT_VISUALIZE:
    arguments->visualize = 1;
    break;

  case ARGP_KEY_ARG:
    if (state->arg_num > 0)
      argp_usage(state); // too many args
    arguments->input_file = arg;
    break;

  case ARGP_KEY_NO_ARGS:
    argp_usage(state); // no args
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

struct argp argp = {options, parse_opt, args_doc, doc};

#endif
