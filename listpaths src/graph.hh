#ifndef GRAPH_HH
#define GRAPH_HH

#include<bits/stdc++.h>
using namespace std;

//#include "common-headers.hh"

class Graph;
class Callstring;

class Opd {
	int value;
	string name;
	OpdType opd_type;	

  public: 
	Opd ()	{ }
	Opd (int n) { value = n; opd_type = num_opd; } 
        Opd (string s) { name = s; opd_type = var_opd; } 
        ~Opd() { }
	string get_opd_string();
};


class Node {
	int node_id; // this is internal
	Graph * cfg;

	StmtType node_type;

	int label;  // this is the number found in the input
	string lhs;
	Opd rhs1;
	Opd rhs2;
	string op;

    public:
	Node (int n, string l, Opd r1, string o, Opd r2);
	Node (int n, string l, Opd r1);
	Node (int n, string l);
	~Node() {}
	int get_node_id();
	void set_node_id(int id);
	int get_node_label();
	void print_node();
	StmtType get_node_type();
	string get_callee_name();
	void set_cfg_of_node(Graph *g) {cfg = g; }
	Graph * get_cfg_of_node() { return cfg; }

};

class Path {
	vector<pair<Callstring, int>> path;

    public:
	Path() {}
	~Path() {}
	void print_path_using_node_labels();
	void add_cs_node(Callstring cs, int node_id);
};

class Graph {
	vector<int> succ[NO_OF_NODES];
	map<int, Node *>  node_id_to_node_map;
	map<int, Node *>  node_label_to_node_map;
	string proc_name;

   public:
	Graph() {}
	~Graph() {}
	void add_node(Node * u);
	Node * get_node_from_id(int id);
	Node * get_node_from_label(int lab);
	void add_edge(int label1, int label2);
	void print_graph(int V);
	//void print_paths(vector<int> path, int node, int length);
	void build_list_of_paths(Path path, int node_id, int length, Callstring cs, int depth);
	int get_succ_count(Node * node);
	vector<Node*> get_successors(Node *node);
	void set_proc_name(string s) { proc_name = s; }
	string get_proc_name() { return proc_name; }
	vector<int> get_succ_vector(int n_id) { return succ[n_id]; }


};

class Procedure {
	string name;
	Graph * cfg;
	int node_count;

   public:
	Procedure(string s) { name = s; node_count = 0; cfg = new Graph(); cfg->set_proc_name(s); }
	~Procedure() { }
	void add_node(Node * nptr);
	void add_edge(int label1, int label2);
	void print_cfg();
	void print_path_list(Callstring cs, int length, int depth);
	bool is_main_proc() { return name == "main"; }
	string get_proc_name() { return name; }
	Graph * get_proc_cfg() { return cfg; }
};

class Callstring {
	vector<int> callstring;
  public:
	Callstring(int n) { callstring.push_back(n); }
	Callstring(vector<int> s) { callstring = s; }
	~Callstring() { }
	Callstring append_call_node(int n);
	void print_callstring();
	int size() {return callstring.size(); }
	int recursion_depth(); 
	Callstring remove_last_call_node(); 
	int get_last_call_node();
};

class Program {
	vector <Procedure *> proc_list;
	vector<Path> path_list;
	map <int, Node *> global_label_to_node_map;

  public: 
	Program() { }
	~Program() { }
	void print_program();
	void print_path_list(int length, int depth);
	void add_procedure(Procedure *p);
	Graph * get_cfg_of_proc(string name);
	void add_label_and_node(int label, Node *n);
	void add_path_to_path_list(Path path);
	Node * get_node_from_label(int lab);
	void build_path_list(int length, int depth);
	
	int find_node_id_of_call_node(int label);
	Graph * find_cfg_of_node_label(int label);
};

#endif
