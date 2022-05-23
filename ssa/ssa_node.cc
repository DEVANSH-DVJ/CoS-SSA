#include "../headers.hh"

using namespace std;

SSA_Node::SSA_Node(SSA_NodeType type, int node_id) {
  this->type = type;
  this->node_id = node_id;

  this->parent_proc = "";
  this->metas = new map<int, SSA_Meta *>();
  this->in_edges = new map<int, SSA_Edge *>();
  this->out_edges = new map<int, SSA_Edge *>();
}

SSA_Node::~SSA_Node() {
  for (map<int, SSA_Meta *>::iterator it = this->metas->begin();
       it != this->metas->end(); ++it) {
    delete it->second;
  }
  delete this->metas;
  delete this->in_edges;
  delete this->out_edges;
}
