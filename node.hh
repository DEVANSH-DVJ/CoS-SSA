#ifndef _NODE_HH_
#define _NODE_HH_

#include <fstream>
#include <iostream>
#include <string>

#include "opd.hh"

using namespace std;

typedef enum { start_stmt, end_stmt, call_stmt, copy_stmt, expr_stmt } StmtType;

class Node;

class TerminalNode;
class CallNode;
class CopyNode;
class ExprNode;

class Node {
public:
  int node_num;
  StmtType type;
  string stmt;

  Node(int node_num, StmtType type);
  ~Node();

  void visualize();
};

class TerminalNode : public Node {
public:
  string name;

  TerminalNode(int node_num, StmtType type, string name);
  ~TerminalNode();
};

class CallNode : public Node {
public:
  string callee;

  CallNode(int node_num, string callee);
  ~CallNode();
};

class CopyNode : public Node {
public:
  Opd *lhs;
  Opd *rhs;

  CopyNode(int node_num, Opd *lhs, Opd *rhs);
  ~CopyNode();
};

class ExprNode : public Node {
public:
  string op;
  Opd *lopd;
  Opd *ropd1;
  Opd *ropd2;

  ExprNode(int node_num, string op, Opd *lopd, Opd *ropd1, Opd *ropd2);
  ~ExprNode();
};

#endif
