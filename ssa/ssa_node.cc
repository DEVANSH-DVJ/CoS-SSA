#include "../headers.hh"

using namespace std;

SSA_Node::SSA_Node(SSA_NodeType type, int node_id) {
  this->type = type;
  this->node_id = node_id;

  this->parent_proc = "";
  this->metas = new map<int, SSA_Meta *>();
}

SSA_Node::~SSA_Node() {
  for (map<int, SSA_Meta *>::iterator it = this->metas->begin();
       it != this->metas->end(); ++it) {
    delete it->second;
  }
  delete this->metas;
}
