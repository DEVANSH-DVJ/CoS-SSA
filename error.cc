#include "error.hh"

extern FILE *cfg_out;
extern FILE *ssa_out;

extern string dot_file;
extern fstream *dot_fd;

void cleanup() {
  cfg_out = freopen(NULL, "w", cfg_out);
  ssa_out = freopen(NULL, "w", ssa_out);

  dot_fd->close();
  dot_fd->open(dot_file.c_str(), ios::out | ios::trunc);

  fclose(cfg_out);
  fclose(ssa_out);
  dot_fd->close();

  exit(1);
}

void check_invariant_underlying_function(bool condition, string error_message,
                                         int line, string file) {
  if (!condition) {
    cerr << "\nInternal error: " << error_message << " (Invariant at line "
         << line << ", file " << file << ").\n";
    print_stacktrace();
    cleanup();
  }
}

void report_violation_and_abort(bool condition, string error_message) {
  if (!condition) {
    cerr << "Semantic Error: " << error_message << "\n";
    cleanup();
  }
}
