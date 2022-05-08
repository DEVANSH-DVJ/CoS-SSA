#include "procedure.hh"

extern void cleanup(const char *);

Procedure::Procedure() {
  this->name = "";
  this->nodes = new list<Node *>();
  this->node_map = new map<int, Node *>();
  this->edges = new list<Edge *>();
}

Procedure::~Procedure() {
  delete this->nodes;
  delete this->node_map;
  delete this->edges;
}

void Procedure::basic_check() {
  if (this->nodes->size() < 2) {
    cleanup("Procedure has less than 2 nodes.");
  }

  TerminalNode *start_node = (TerminalNode *)*this->nodes->begin();
  TerminalNode *end_node = (TerminalNode *)*this->nodes->end();

  if (start_node->type != start_stmt) {
    cleanup("Procedure has no start node.");
  }
  if (end_node->type != end_stmt) {
    cleanup("Procedure has no end node.");
  }
  if (start_node->name != end_node->name) {
    cleanup("Start and end nodes are not named the same.");
  }

  this->name = start_node->name;

  for (list<Node *>::iterator it = this->nodes->begin();
       it != this->nodes->end(); ++it) {
    node_map->insert(pair<int, Node *>((*it)->node_num, *it));
  }

  for (list<Edge *>::iterator it = this->edges->begin();
       it != this->edges->end(); ++it) {
    Edge *edge = *it;
    if (edge->src->node_num == end_node->node_num) {
      cleanup("End node can not be source.");
    }
    if (edge->dst->node_num == start_node->node_num) {
      cleanup("Start node can not be destination.");
    }
    edge->src = node_map->find(edge->src->node_num)->second;
    edge->dst = node_map->find(edge->dst->node_num)->second;
  }
}

void Procedure::visualize() {}
