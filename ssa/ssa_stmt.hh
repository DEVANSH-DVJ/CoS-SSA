#ifndef _SSA_STMT_HH_
#define _SSA_STMT_HH_

#include <string>

#include <list>

class SSA_Opd;

typedef enum {
  SSA_StartStmt,
  SSA_EndStmt,
  SSA_CallStmt,
  SSA_InputStmt,
  SSA_UsevarStmt,
  SSA_ExprStmt,
  SSA_PhiStmt,
} SSA_StmtType;

class SSA_Stmt {
public:
  SSA_StmtType type;

  std::string stmt;

  std::string callee_proc;
  std::string op;
  SSA_Opd *lopd;
  SSA_Opd *ropd1;
  SSA_Opd *ropd2;
  std::list<SSA_Opd *> *ropds;

  // SSA_Stmt(SSA_StmtType type);
  // ~SSA_Stmt();
};

#endif
