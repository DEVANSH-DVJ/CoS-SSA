#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;
extern Program *program;

SSA_Edge::SSA_Edge(int src_id, int dst_id) {
  this->src = program->get_ssa_node(src_id, true);
  this->dst = program->get_ssa_node(dst_id, true);
  this->edge_id = make_pair(src_id, dst_id);
}

SSA_Edge::~SSA_Edge() {}

const pair<int, int> &SSA_Edge::get_edge_id() const { return this->edge_id; }

const SSA_Node *SSA_Edge::get_src() const { return this->src; }

const SSA_Node *SSA_Edge::get_dst() const { return this->dst; }

void SSA_Edge::visualize() const {
  *dot_fd << "\t\tnode_" << this->src->get_node_id() << " -> "
          << "node_" << this->dst->get_node_id() << ";\n";
}
