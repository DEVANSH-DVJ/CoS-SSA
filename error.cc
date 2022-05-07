#include "error.hh"

string Backtrace(int skip = 1);

void check_invariant_underlying_function(bool condition, string error_message,
                                         int line, string file) {
  if (!condition) {
    cerr << "\nInternal error: " << error_message << " (Invariant at line "
         << line << ", file " << file << ").\n";
    print_stacktrace();
    exit(1);
  }
}
