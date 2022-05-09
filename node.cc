#include "node.hh"

extern fstream *dot_fd;
extern void cleanup(const char *);

Node::Node(int node_num, StmtType type) {
  this->node_num = node_num;
  this->type = type;
}
Node::~Node() {}
void Node::visualize() {
  *dot_fd << "\t\tnode_" << this->node_num << " [shape=box, label=\""
          << this->node_num << ". " << this->stmt << "\"];" << endl;
}

TerminalNode::TerminalNode(int node_num, StmtType type, string name)
    : Node(node_num, type) {
  this->name = name;
  switch (this->type) {
  case start_stmt:
    this->stmt = "Start " + this->name;
    break;
  case end_stmt:
    this->stmt = "End " + this->name;
    break;
  default:
    cleanup("Wrong terminal type");
  }
}
TerminalNode::~TerminalNode() {}

CallNode::CallNode(int node_num, string callee) : Node(node_num, call_stmt) {
  this->callee = callee;
  this->stmt = "Call " + this->callee;
}
CallNode::~CallNode() {}

CopyNode::CopyNode(int node_num, Opd *lhs, Opd *rhs)
    : Node(node_num, copy_stmt) {
  this->lhs = lhs;
  this->rhs = rhs;
  this->stmt = this->lhs->to_str() + " = " + this->rhs->to_str();
}
CopyNode::~CopyNode() {}

ExprNode::ExprNode(int node_num, string op, Opd *lopd, Opd *ropd1, Opd *ropd2)
    : Node(node_num, expr_stmt) {
  this->op = op;
  this->lopd = lopd;
  this->ropd1 = ropd1;
  this->ropd2 = ropd2;
  this->stmt = this->lopd->to_str() + " = " + this->ropd1->to_str() + " " +
               this->op + " " + this->ropd2->to_str();
}
ExprNode::~ExprNode() {}
