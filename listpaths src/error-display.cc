#include <execinfo.h>
#include <unistd.h>


#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <string>

using namespace std;

#include "stacktrace.h"
#include "common-headers.hh"

string Backtrace(int skip = 1);

void check_invariant_underlying_function(bool condition, string error_message)
{
	if (!condition)
	{
		cerr << "\nInternal error: " <<  error_message << "\n";

		print_stacktrace();
		//cerr << Backtrace();
		
		exit(1);
	}
}

void report_violation_of_condition(bool condition, string error_message)
{
	if (!condition)
	{
		cerr <<  "Error: " << error_message << "\n";
	}
} 

void report_violation_and_abort(bool condition, string error_message)
{
	if (!condition)
	{
		cerr <<  "Error: " << error_message << "\n";
		exit(1);
	}
} 
