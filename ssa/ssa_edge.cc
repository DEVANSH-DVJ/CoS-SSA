#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;
extern Program *program;

SSA_Edge::SSA_Edge(int src_id, int dst_id) {
  this->src = program->get_ssa_node(src_id);
  this->dst = program->get_ssa_node(dst_id);
  this->edge_id = make_pair(src_id, dst_id);
}

SSA_Edge::~SSA_Edge() {}

void SSA_Edge::visualize() {
  *dot_fd << "\t\tnode_" << this->src->node_id << " -> "
          << "node_" << this->dst->node_id << ";\n";
}
