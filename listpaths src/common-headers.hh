# include <stdio.h>
# include <iostream>
# include <string>
# include <vector>
# include <sstream>
# include <map>
#include <argp.h>

#define NO_OF_NODES 10

typedef enum {bexpr_asgn, copy_stmt, call_stmt} StmtType;
typedef enum {num_opd, var_opd} OpdType;

#include "error-display.hh"
#include "graph.hh"

extern Program program;

#if 0
class Node;
class Graph;
class Path;
class Statement;
#endif
