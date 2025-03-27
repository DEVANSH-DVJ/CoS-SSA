#ifndef _DDG_COMPUTE_HH_
#define _DDG_COMPUTE_HH_

#include "ddg_types.hh"

#include <map>

void ddg_construct();
std::map<QDef, int> ddg_propagate_constants();

#endif
