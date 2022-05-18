#ifndef _ARGPARSE_HH_
#define _ARGPARSE_HH_

#include <argp.h>
#include <cstring>
#include <string>

using namespace std;

enum short_options {
  OPT_TYPE = 0x80,
  OPT_VISUALIZE,
};

enum file_type { FILE_CFG, FILE_SSA, FILE_UNKNOWN };

struct arguments {
  int visualize;
  file_type input_type;
  string input_file;
};

/* Program documentation */
char doc[] = "CS-SSA";

/* Argument description */
char args_doc[] = "[FILE]";

/* Options visible to user */
struct argp_option options[] = {{"type", OPT_TYPE, "TYPE", 0, "Type of input"},
                                {"visualize", OPT_VISUALIZE, 0, 0, "Visualize"},
                                {0}};

error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = (struct arguments *)state->input;

  switch (key) {
  case OPT_TYPE:
    if (arg == NULL) {
      cout << "Need to specify input type" << endl;
      argp_usage(state);
    } else if (strcmp(arg, "cfg") == 0) {
      arguments->input_type = FILE_CFG;
    } else if (strcmp(arg, "ssa") == 0) {
      arguments->input_type = FILE_SSA;
    } else {
      cout << "Unknown input type: " << arg << endl;
      argp_usage(state);
    }
    break;

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

  case ARGP_KEY_END:
    if (arguments->input_type == FILE_UNKNOWN) {
      cout << "Need to specify input type" << endl;
      argp_usage(state);
    }
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

struct argp argp = {options, parse_opt, args_doc, doc};

#endif
