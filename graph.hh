#ifndef GRAPH_HH
#define GRAPH_HH

#include<bits/stdc++.h>
using namespace std;

//#include "common-headers.hh"

set<string> s_intersect(set<string> s1, set<string> s2);

class Graph;

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
	set<string> kills;

	public:
	Node (int n, StmtType stmt_type);
	Node (int n, string l, Opd r1, string o, Opd r2);
	Node (int n, string l, Opd r1);
	Node (int n, string l);
	~Node() {}
	int get_node_id() { return node_id; }
	void set_node_id(int id) { node_id = id; }
	int get_node_label() { return label; }
	void print_node();
	StmtType get_node_type() { return node_type; }
	string get_callee_name();
	string get_lhs();
	void set_cfg_of_node(Graph *g) { cfg = g; }
	Graph * get_cfg_of_node() { return cfg; }

};


// class HoistableDef {
// 	Node *node;
// 	set<int> call_sites;

// 	public:
// 		HoistableDef(Node *u, set<int> cs);
// 		~HoistableDef() {}
// 		int get_def_label() const;
// 		Node *get_node() { return node; }
// 		set<int> get_call_sites() const;
// 		void add_call_site(int site);
// 		bool operator<(const HoistableDef &hd) const;
// };

class qD {
	int label;
	Node * node;
	vector<int> call_seq;

public:
	qD(int l, Node* n);
	~qD(){}
	string get_var();
	void append_call_site(int call_site);
	bool operator<(const qD &d) const;
	int get_label() const { return label; }
	vector<int> get_call_seq() const { return call_seq; }
};

class qDV {
	int label;
	Node* node;
	vector<int> call_seq;
	set<string> uexp_var;

public:
	qDV(int l, Node *n, set<string> vars);
	~qDV(){}
	bool is_var_exp(string var);
	void kill_var(string var);
	void append_call_site(int call_site);
	bool is_upwards_exp();
	bool operator<(const qDV &d) const;
	int get_label() const { return label; }
	vector<int> get_call_seq() const { return call_seq; }
	set<string> get_uexp_vars() const { return uexp_var; }
};

class Graph
{
	vector<int> succ[NO_OF_NODES];
	vector<int> pred[NO_OF_NODES];
	map<int, Node *> node_id_to_node_map;
	map<int, Node *>  node_label_to_node_map;
	string proc_name;
	Node *start_node, *end_node;

	public:
	Graph() {}
	~Graph() {}
	void add_node(Node * u);
	Node * get_node_from_id(int id);
	Node * get_node_from_label(int lab);
	void add_edge(int label1, int label2);
	void print_graph(int V);
	void set_proc_name(string s) { proc_name = s; }
	string get_proc_name() { return proc_name; }
	vector<int> get_succ_vector(int n_id) { return succ[n_id]; }
	vector<int> get_pred_vector(int n_id) { return pred[n_id]; }
	set<string> get_all_lhs_vars();
	set<string> compute_kill_def();
	set<qD> compute_dexp_def();
	set<qDV> compute_uexp_def();
};

class Procedure;

class CallEdge
{
	int label;
	string caller;
	string callee;

public:
	CallEdge(int lab, string callee, string caller);
	CallEdge() {}
	~CallEdge() {}
	Procedure *get_callee_proc();
	Procedure *get_caller_proc();
	int get_call_site_label() { return label; }
	void print_edge();
};

class Procedure {
	int proc_id;
	string name;
	Graph * cfg;
	int node_count;
	set<string> killDef;
	set<qD> dexpDef;
	set<qDV> uexpDef;
	vector<CallEdge> *pred_procs;
	vector<CallEdge> *succ_procs;

	// set<HoistableDef> hoistable_defs;

public:
	Procedure(string s) { name = s;
		node_count = 0;
		cfg = new Graph();
		cfg->set_proc_name(s);
		pred_procs = new vector<CallEdge>();
		succ_procs = new vector<CallEdge>();
	}
	~Procedure() { }
	void add_node(Node * nptr);
	void add_edge(int label1, int label2);
	void add_pred_proc(CallEdge e);	// adds call edge to pred_procs of current procedure
	void add_call_edges_to_preds(); //adds call edges to pred_procs
									//of successors of the current procedure
	void add_succ_proc(int lab, string proc_name);
	void set_proc_id(int id) { proc_id = id; }
	bool is_main_proc() { return name == "main"; }
	int get_proc_id() { return proc_id; }
	set<string> get_kill_def() { return killDef; }
	set<qD> get_dexp_def() { return dexpDef; }
	set<qDV> get_uexp_def() { return uexpDef; }
	vector<CallEdge> *get_pred_procs() { return pred_procs; }
	vector<CallEdge> *get_succ_procs() { return succ_procs; }
	int get_succ_count() { return succ_procs->size(); }
	string get_proc_name() { return name; }
	Graph * get_proc_cfg() { return cfg; }
	void compute() {
		killDef = cfg->compute_kill_def();
		// dexpDef = cfg->compute_dexp_def();
		// uexpDef = cfg->compute_uexp_def();
	}
	void print_cfg();
	void print_kill_def();
	//TODO: print functions
	void print_dexp_def();
	void print_uexp_def();
	// void gen_hoistable_defs();
	// set<HoistableDef> get_hoistable_defs();
	// bool add_hoistable_defs(set<HoistableDef> h_defs, int call_site);
	// void print_hoistable_defs();
	// void print_hoisting_region();
};


class Program {
	vector <Procedure *> proc_list;
	map<string, Procedure *> proc_name_to_proc_map;
	map<int, Procedure *> proc_id_to_proc_map;
	map <int, Node *> global_label_to_node_map;
	int proc_count;
	set<string> all_lhs_vars;
	// map<int, set<int>> hoisting_region_map; 	//label of definition node to hoisting region

public:
	Program() { proc_count = 0; }
	~Program() { }
	void print_program();
	void print_kill_def();
	void add_procedure(Procedure *p);
	Procedure *get_proc_from_name(string name);
	Graph *get_cfg_of_proc(string name);
	Node *get_node_from_label(int lab);
	set<string> get_all_lhs_vars() { return all_lhs_vars; }
	void add_label_and_node(int label, Node *n);
	int find_node_id_of_call_node(int label);
	Graph *find_cfg_of_node_label(int label);
	void build_all_lhs_vars();
	void build_pred_procs();
	void build_defs();
	void compute();
	
	// set<int> get_hoisting_region_from_label(int label);
	// void build_hoistable_defs();
	// void print_hoistable_defs();
	// void build_hoisting_region(); 	//internally calls build_hoistable_defs
	// void print_hoisting_region();
};

#endif
