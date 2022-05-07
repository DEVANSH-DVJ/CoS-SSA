#include "node.hh"

extern void cleanup(const char *);

Node::Node(int node_num, StmtType type) {
  this->node_num = node_num;
  this->type = type;
}
Node::~Node() {}

TerminalNode::TerminalNode(int node_num, StmtType type, string name)
    : Node(node_num, type) {
  this->name = name;
}
TerminalNode::~TerminalNode() {}
void TerminalNode::visualize() {
  switch (this->type) {
  case start_stmt:
    cout << "Start " << this->name << endl;
    break;
  case end_stmt:
    cout << "End " << this->name << endl;
    break;
  default:
    cleanup("Unknown statement type");
  }
}

CallNode::CallNode(int node_num, string callee) : Node(node_num, call_stmt) {
  this->callee = callee;
}
CallNode::~CallNode() {}
void CallNode::visualize() { cout << this->callee; }

CopyNode::CopyNode(int node_num, Opd *lhs, Opd *rhs) : Node(node_num, copy_stmt) {
  this->lhs = lhs;
  this->rhs = rhs;
}
CopyNode::~CopyNode() {}
void CopyNode::visualize() {
  this->lhs->visualize();
  cout << " = ";
  this->rhs->visualize();
}

ExprNode::ExprNode(int node_num, string op, Opd *lopd, Opd *ropd1, Opd *ropd2)
    : Node(node_num, expr_stmt) {
  this->op = op;
  this->lopd = lopd;
  this->ropd1 = ropd1;
  this->ropd2 = ropd2;
}
ExprNode::~ExprNode() {}
void ExprNode::visualize() {
  this->lopd->visualize();
  cout << " = ";
  this->ropd1->visualize();
  cout << " " << this->op << " ";
  this->ropd2->visualize();
}
