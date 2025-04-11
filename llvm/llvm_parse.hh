#ifndef _LLVM_PARSE_HH_
#define _LLVM_PARSE_HH_

#include <llvm/IR/Value.h>
#include <vector>

void llvm_set_in(std::string file);
std::vector<llvm::Value*> llvm_parse();

#endif
