#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;
extern Program *program;

CFG_Edge::CFG_Edge(int src_id, int dst_id) {
  this->src = program->cfg_nodes->find(src_id)->second;
  this->dst = program->cfg_nodes->find(dst_id)->second;
  this->edge_id = make_pair(src_id, dst_id);
}

CFG_Edge::~CFG_Edge() {}

void CFG_Edge::visualize() {
  *dot_fd << "\t\tnode_" << this->src->node_id << " -> "
          << "node_" << this->dst->node_id << ";\n";
}
