#ifndef _EDGE_HH_
#define _EDGE_HH_

#include <fstream>
#include <iostream>
#include <string>

#include "node.hh"

using namespace std;

class Edge;

class Edge {
public:
  int src_num;
  int dst_num;
  Node *src;
  Node *dst;

  Edge(int src_num, int dst_num);
  ~Edge();

  void visualize();
};

#endif
