#include "../headers.hh"

using namespace std;

CFG_Node::CFG_Node(CFG_NodeType type, int node_id) {
  this->type = type;
  this->node_id = node_id;
  this->in_edges = new std::map<int, CFG_Edge *>();
  this->out_edges = new std::map<int, CFG_Edge *>();
}

CFG_Node::~CFG_Node() {
  delete this->in_edges;
  delete this->out_edges;
}
