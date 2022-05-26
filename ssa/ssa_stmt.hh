#ifndef _SSA_STMT_HH_
#define _SSA_STMT_HH_

#include <string>

#include <list>

class SSA_Opd;

typedef enum {
  SSA_AssignStmt,
  SSA_PhiStmt,
} SSA_StmtType;

class SSA_Stmt {
public:
  /* Always initialized: by constructor */
  SSA_StmtType type;

  /* Always initialized: post parsing*/
  std::string stmt;

  /* Conditionally initialized: post parsing */
  std::string op;
  SSA_Opd *lopd;
  SSA_Opd *ropd1;
  SSA_Opd *ropd2;
  std::list<SSA_Opd *> *ropds;

  /* Constructors and Destructor */
  SSA_Stmt(SSA_StmtType type, std::string op, SSA_Opd *lopd, SSA_Opd *ropd1,
           SSA_Opd *ropd2);
  SSA_Stmt(SSA_StmtType type, SSA_Opd *lopd, std::list<SSA_Opd *> *ropds);
  ~SSA_Stmt();
};

#endif
