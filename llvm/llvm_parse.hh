#ifndef _LLVM_PARSE_HH_
#define _LLVM_PARSE_HH_

#include <llvm/IR/Value.h>
#include <map>

void llvm_set_in(std::string file);
std::map<int, llvm::Value*> llvm_parse();

#endif
