#ifndef ERROR_DISPLAY_HH
#define ERROR_DISPLAY_HH

#include <sstream>
#include <string>

#define NO_FILE_LINE -1

#define CONTROL_SHOULD_NOT_REACH false

using namespace std;

void report_violation_of_condition(bool condition, string s);
void report_violation_and_abort(bool condition, string s);
void check_invariant_underlying_function(bool condition, string error_message);

#define CHECK_INVARIANT(x,y) \
     {  stringstream ___new___string___; \
        ___new___string___ << y << " (Invariant at line " << __LINE__ <<  ", file " << __FILE__ << ").\n"; \
        check_invariant_underlying_function(x, ___new___string___.str()); \
     }

#define CHECK_INPUT(condition, error_message)							\
	report_violation_of_condition(condition, error_message);

#define CHECK_INPUT_AND_ABORT(condition, error_message)							\
	report_violation_and_abort(condition, error_message);

#endif
