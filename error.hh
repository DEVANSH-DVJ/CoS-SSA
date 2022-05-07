#ifndef _ERROR_HH_
#define _ERROR_HH_

#include <iostream>
#include <string>

#include "stacktrace.h"

using namespace std;

void check_invariant_underlying_function(bool condition, string error_message,
                                         int line, string file);

#define CHECK_INVARIANT(x, error_message)                                      \
  check_invariant_underlying_function(x, error_message, __LINE__, __FILE__);

#endif
