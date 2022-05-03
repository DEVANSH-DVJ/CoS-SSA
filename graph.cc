#include "common-headers.hh"

set<string> s_intersect(set<string> s1, set<string> s2){
	set<string> new_set = set<string>();
	set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(new_set, new_set.begin()));
	return new_set;
}

string Node::get_callee_name()
{
	CHECK_INVARIANT (node_type == call_stmt, "Call name is defined only for a call statement");

	return lhs;
}

string Node:: get_lhs(){
	CHECK_INVARIANT(node_type == bexpr_asgn || node_type==copy_stmt, "lhs is defined only for a definition");
	return lhs;
}

Node::Node(int n, StmtType stmt_type){
	// For start and end statements
	stringstream msg;
	msg << "Label " << n << " must be smaller than the number of nodes (" << to_string(NO_OF_NODES) << ")";
	CHECK_INPUT_AND_ABORT(n < NO_OF_NODES, msg.str());
	CHECK_INPUT_AND_ABORT(n > 0, "Node label must be greater than 0");
	label = n;
	node_type = stmt_type;
}

Node::Node(int n, string l, Opd r1, string o, Opd r2)
{
	// l is LHS here and not the label
	
	stringstream msg;
	msg << "Label " << n << " must be smaller than the number of nodes (" << to_string(NO_OF_NODES) << ")";
	CHECK_INPUT_AND_ABORT (n < NO_OF_NODES, msg.str());
	CHECK_INPUT_AND_ABORT (n > 0, "Node label must be greater than 0");
	node_type = bexpr_asgn;
	label = n;
	lhs = l;
	rhs1 = r1;
	rhs2 = r2;
	op = o;
	kills.insert(lhs);
}

Node::Node(int n, string l, Opd r1)
{
	stringstream msg;
	msg << "Label " << n << " must be smaller than the number of nodes (" << to_string(NO_OF_NODES) << ")";
	CHECK_INPUT_AND_ABORT (n < NO_OF_NODES, msg.str());
	CHECK_INPUT_AND_ABORT (n > 0, "Node label must be greater than 0");
	node_type = copy_stmt;
	label = n;
	lhs = l;
	rhs1 = r1;
	kills.insert(lhs);
}

Node::Node(int n, string l)
{
	stringstream msg;
	msg << "Label " << n << " must be smaller than the number of nodes (" << to_string(NO_OF_NODES) << ")";
	CHECK_INPUT_AND_ABORT (n < NO_OF_NODES, msg.str());
	CHECK_INPUT_AND_ABORT (n > 0, "Node label must be greater than 0");
	node_type = call_stmt;
	label = n;
	lhs = l;
}

void Node::print_node()
{
	if (node_type == bexpr_asgn)
		cout << label << ": " << lhs << " = " << rhs1.get_opd_string() << " " << op << " " << rhs2.get_opd_string() ;
	else if (node_type == copy_stmt)
		cout << label << ": " << lhs << " = " << rhs1.get_opd_string() ;
	else if (node_type == call_stmt)
		cout << label << ": call " << lhs ;
	else if(node_type == start_stmt)
		cout << label << ": START " << this->get_cfg_of_node()->get_proc_name();
	else if(node_type == end_stmt)
		cout << label << ": END";
}

// int HoistableDef::get_def_label() const{
// 	return node->get_node_label();
// }

// void HoistableDef:: add_call_site(int site){
// 	call_sites.insert(site);
// }

// set<int> HoistableDef:: get_call_sites() const{
// 	return call_sites;
// }

// bool HoistableDef::operator<(const HoistableDef& hd) const{
// 	return (this->get_def_label() < hd.get_def_label()) ||
// 		(this->get_def_label() == hd.get_def_label() &&
// 		this->get_call_sites() < hd.get_call_sites());
// }

// HoistableDef::HoistableDef(Node* u, set<int>cs){
// 	node = u;
// 	call_sites = cs;
// }

qD::qD(int l, Node* n){
	label = l;
	node = n;
	call_seq = vector<int>();
}

string qD:: get_var(){
	return node->get_lhs();
}

void qD::append_call_site(int call_site){
	call_seq.push_back(call_site);
}

bool qD::operator<(const qD& d) const{
	return (this->label < d.get_label())|| (this->label==d.get_label() && this->call_seq<d.get_call_seq());
}

qDV::qDV(int l, Node* n, set<string>vars){
	label = l;
	node = n;
	uexp_var = vars;
}

bool qDV::is_var_exp(string var){
	//Checks if the given variable is upwards exposed in the qDV
	return uexp_var.find(var) != uexp_var.end();
}

void qDV::append_call_site(int call_site){
	call_seq.push_back(call_site);
}

void qDV::kill_var(string var){
	//Removes a variable from upwards exposed set
	uexp_var.erase(var);
}

bool qDV::is_upwards_exp(){
	//Checks if the definition has any variable that is upwards exposed
	return uexp_var.size() > 0;
}

bool qDV::operator<(const qDV &d) const
{
	return (this->label < d.get_label()) || (this->label == d.get_label() && this->call_seq < d.get_call_seq()) 
	|| (this->label == d.get_label() && this->call_seq == d.get_call_seq() && this->uexp_var<d.get_uexp_vars());
}

void Graph::add_edge(int l1, int l2)
{
	int id1 = get_node_from_label(l1)->get_node_id();
	int id2 = get_node_from_label(l2)->get_node_id();

	CHECK_INPUT_AND_ABORT (l1 > 0, "Node label must be greater than 0");
	CHECK_INPUT_AND_ABORT (l2 > 0, "Node label must be greater than 0");
	succ[id1].push_back(id2);
	pred[id2].push_back(id1);
}

void Graph::add_node(Node *u)
{
	stringstream msg1, msg2;
	int id = u->get_node_id();
	int lab = u->get_node_label();

	msg1 << "Node Id " << id << " must be smaller than the number of nodes (" << to_string(NO_OF_NODES) << ")";
	CHECK_INVARIANT (id < NO_OF_NODES, msg1.str());
	msg2 << "Label " << lab << " must be smaller than the number of nodes (" << to_string(NO_OF_NODES) << ")";
	CHECK_INPUT_AND_ABORT (lab < NO_OF_NODES, msg2.str());
	node_id_to_node_map[id] = u;
	node_label_to_node_map[lab] = u;

	if (u->get_node_type() == start_stmt)
	{
		start_node = u;
	}
	else if (u->get_node_type() == end_stmt)
	{
		end_node = u;
	}
	u->set_cfg_of_node(this);
	program.add_label_and_node(lab, u);
}

Node * Graph::get_node_from_id(int id)
{
	stringstream msg;

	msg << "Node Id " << id << " must be smaller than the number of nodes (" << to_string(NO_OF_NODES) << ")";
	CHECK_INVARIANT (id < NO_OF_NODES, msg.str());
	return node_id_to_node_map[id];
}

Node * Graph::get_node_from_label(int lab)
{
	stringstream msg;
	msg << "Label " << lab << " must be smaller than the number of nodes (" << to_string(NO_OF_NODES) << ")";
	CHECK_INPUT_AND_ABORT (lab < NO_OF_NODES, msg.str());
	return node_label_to_node_map[lab];
}



void Graph::print_graph(int node_count)
{
	for (int v = 0; v < node_count; ++v)
	{
		cout << "Node ";
		get_node_from_id(v)->print_node();
		cout << "\t | Successor nodes: ";
		for (auto x : succ[v])
			cout  << get_node_from_id(x)->get_node_label() << ", ";
		printf("\n");
	}
}

set<string> Graph::get_all_lhs_vars(){
	set<string> all_lhs_vars;
	for(auto iter: node_id_to_node_map){
		Node *node = iter.second;
		if(node->get_node_type() == bexpr_asgn || node->get_node_type() == copy_stmt){
			all_lhs_vars.insert(node->get_lhs());
		}
	}
	return all_lhs_vars;
}

set<string> Graph::compute_kill_def(){
	//Computes killDef for a procedure
	map<int, set<string>> node_id_to_killIn, node_id_to_killOut;
	for(auto iter: node_id_to_node_map){
		node_id_to_killIn[iter.first] = program.get_all_lhs_vars();
		node_id_to_killOut[iter.first] = program.get_all_lhs_vars();
	}
	bool changed = true;
	set<string> EMPTY_SET = set<string>();
	while (changed)
	{
		changed = false;
		for(auto iter:node_id_to_node_map){
			Node *n = iter.second;
			int node_id = iter.first;
			if (n->get_node_type() == start_stmt)
			{
				if(node_id_to_killIn[node_id] != EMPTY_SET){
					node_id_to_killIn[node_id] = EMPTY_SET;
					changed = true;
				}
				if(node_id_to_killOut[node_id] != EMPTY_SET){
					node_id_to_killOut[node_id] = EMPTY_SET;
					changed = true;
				}
			}
			else{
				// Computing killIn
				vector<int> parent_nodes = pred[node_id];
				set<string> new_killIn = program.get_all_lhs_vars();
				for (int parent_id : parent_nodes)
				{
					new_killIn = s_intersect(new_killIn, node_id_to_killOut[parent_id]);
				}
				if (node_id_to_killIn[node_id] != new_killIn)
				{
					node_id_to_killIn[node_id] = new_killIn;
					changed = true;
				}

				// Computing killOut
				set<string> new_killOut = new_killIn;
				if(n->get_node_type() == bexpr_asgn || n->get_node_type() == copy_stmt){
					new_killOut.insert(n->get_lhs());
				}
				else if(n->get_node_type() == call_stmt){
					string callee = n->get_callee_name();
					Procedure *callee_proc = program.get_proc_from_name(callee);
					set<string> callee_kill_def = callee_proc->get_kill_def();
					new_killOut.insert(callee_kill_def.begin(), callee_kill_def.end());
				}

				if(node_id_to_killOut[node_id] != new_killOut){
					node_id_to_killOut[node_id] = new_killOut;
					changed = true;
				}
			}
		}
	}
	return node_id_to_killOut[end_node->get_node_id()];
}

set<qD> Graph::compute_dexp_def()
{	//Computes dexpDef for a procedure
	set<qD> EMPTY_SET = set<qD>();
	map<int, set<qD>> node_id_to_reachIn, node_id_to_reachOut;
	for (auto iter : node_id_to_node_map)
	{
		node_id_to_reachIn[iter.first] = EMPTY_SET;
		node_id_to_reachOut[iter.first] = EMPTY_SET;
	}
	bool changed = true;
	while (changed)
	{
		changed = false;

		// for (auto iter : node_id_to_node_map)
		// {
		// 	Node *n = iter.second;
		// 	int node_id = iter.first;
		// 	if (n->get_node_type() == start_stmt)
		// 	{
		// 		if (node_id_to_killIn[node_id] != EMPTY_SET)
		// 		{
		// 			node_id_to_killIn[node_id] = EMPTY_SET;
		// 			changed = true;
		// 		}
		// 		if (node_id_to_killOut[node_id] != EMPTY_SET)
		// 		{
		// 			node_id_to_killOut[node_id] = EMPTY_SET;
		// 			changed = true;
		// 		}
		// 	}
		// 	else
		// 	{
		// 		// Computing killIn
		// 		vector<int> parent_nodes = pred[node_id];
		// 		set<string> new_killIn = program.get_all_lhs_vars();
		// 		for (int parent_id : parent_nodes)
		// 		{
		// 			new_killIn = s_intersect(new_killIn, node_id_to_killOut[parent_id]);
		// 		}
		// 		if (node_id_to_killIn[node_id] != new_killIn)
		// 		{
		// 			node_id_to_killIn[node_id] = new_killIn;
		// 			changed = true;
		// 		}

		// 		// Computing killOut
		// 		set<string> new_killOut = new_killIn;
		// 		if (n->get_node_type() == bexpr_asgn || n->get_node_type() == copy_stmt)
		// 		{
		// 			new_killOut.insert(n->get_lhs());
		// 		}
		// 		else if (n->get_node_type() == call_stmt)
		// 		{
		// 			string callee = n->get_callee_name();
		// 			Procedure *callee_proc = program.get_proc_from_name(callee);
		// 			set<string> callee_kill_def = callee_proc->get_kill_def();
		// 			new_killOut.insert(callee_kill_def.begin(), callee_kill_def.end());
		// 		}

		// 		if (node_id_to_killOut[node_id] != new_killOut)
		// 		{
		// 			node_id_to_killOut[node_id] = new_killOut;
		// 			changed = true;
		// 		}
		// 	}
		// }
	}
	return set<qD>();
}

Procedure *CallEdge::get_callee_proc()
{
	return program.get_proc_from_name(callee);
}

Procedure *CallEdge::get_caller_proc()
{
	return program.get_proc_from_name(caller);
}

void CallEdge::print_edge(){
	cout << caller << "->" << label << "->" << callee << endl;
}

CallEdge:: CallEdge(int lab, string caller, string callee){
	label = lab;
	this->caller = caller;
	this->callee = callee;
}

void Procedure::add_edge(int label1, int label2)
{
	cfg->add_edge(label1, label2);
}

void Procedure::add_node(Node *n)
{
	n->set_node_id(node_count++);
	cfg->add_node(n);
}

void Procedure::add_pred_proc(CallEdge e){
	pred_procs->push_back(e);
}

void Procedure::add_call_edges_to_preds(){
	for(auto e: *succ_procs){
		e.get_callee_proc()->add_pred_proc(e);
	}
}

void Procedure::add_succ_proc(int lab, string proc_name)
{
	succ_procs->push_back(CallEdge(lab, name, proc_name));
}

void Procedure::print_cfg()
{
	cout << "============================\n";
	cout << "Begin Procedure " << name << endl;
	cout << "----------------------------\n";
	cfg->print_graph(node_count);
	cout << "----------------------------\n";
	cout << "End Procedure " << name << endl;
	cout << "============================\n";
}

void Procedure::print_kill_def(){
	cout << name << ": ";
	for(auto var: killDef){
		cout << var << " ";
	}
	cout << endl;
}

// set<HoistableDef> Procedure:: get_hoistable_defs(){
// 	return hoistable_defs;
// }


// void Procedure::gen_hoistable_defs(){
// 	for (int i = 0; i < node_count; i++){
// 		Node *node = cfg->get_node_from_id(i);
// 		if(node->get_node_type() != call_stmt){
// 			hoistable_defs.insert(HoistableDef(node, set<int>()));
// 		}
// 	}
// }

// bool Procedure::add_hoistable_defs(set<HoistableDef>h_defs, int call_site){
// 	int num_hoistable_def = hoistable_defs.size();
// 	for (auto def : h_defs)
// 	{
// 		def.add_call_site(call_site);
// 		hoistable_defs.insert(def);
// 	}
// 	return hoistable_defs.size() > num_hoistable_def;
// }

// void Procedure::print_hoistable_defs(){
// 	for(auto hd: hoistable_defs){
// 		hd.get_node()->print_node();
// 		cout << "\t\t C={";
// 		for (int m : hd.get_call_sites())
// 		{
// 			cout << m << ",";
// 		}
		
// 		cout << "}" <<endl;
// 	}
// }

// void Procedure::print_hoisting_region(){
// 	for (int i = 0; i<node_count; i++){
// 		Node *node = get_proc_cfg()->get_node_from_id(i);
// 		if(node->get_node_type() != call_stmt){
// 			node->print_node();
// 			cout << "\t\t C = {";
// 			for(auto call_site: program.get_hoisting_region_from_label(node->get_node_label())){
// 				cout << call_site << ",";
// 			}
// 			cout << "}" << endl;
// 		}
// 	}
// }


string Opd::get_opd_string()
{
	if (opd_type == num_opd)
		return to_string(value);
	else if (opd_type == var_opd)
		return name;
	else{
		CHECK_INVARIANT(false, "Wrong Opd Type");
		return "";
	}
}

void Program::add_procedure(Procedure *p)
{
	p->set_proc_id(proc_count++);
	proc_id_to_proc_map[p->get_proc_id()] = p;
	proc_list.push_back(p);
	proc_name_to_proc_map[p->get_proc_name()] = p;
}

void Program::print_program()
{
	for (auto p : proc_list)
		p->print_cfg();
}

void Program::print_kill_def(){
	for (auto p: proc_list){
		p->print_kill_def();
	}
}

Procedure * Program:: get_proc_from_name(string name){
	return proc_name_to_proc_map[name];
}

void Program::build_all_lhs_vars(){
	for(auto proc:proc_list){
		set<string> lhs_vars = proc->get_proc_cfg()->get_all_lhs_vars();
		all_lhs_vars.insert(lhs_vars.begin(), lhs_vars.end());
	}
}

void Program:: build_pred_procs(){
	for(auto p:proc_list){
		p->add_call_edges_to_preds();
	}
}

void Program::build_defs(){
	queue<Procedure *> active;
	map<string, int> count_of_succ_processed;
	for (auto proc : proc_list)
	{
		if(proc->get_succ_procs()->size() == 0){
			active.push(proc);
			count_of_succ_processed[proc->get_proc_name()] = 0;
		}
	}
	while(active.size()){
		auto proc = active.front();
		active.pop();
		proc->compute();
		for(auto edge: *(proc->get_pred_procs())){
			Procedure* parent_proc = edge.get_caller_proc();
			count_of_succ_processed[parent_proc->get_proc_name()] += 1;
			if (count_of_succ_processed[parent_proc->get_proc_name()]==parent_proc->get_succ_count())
			{
				active.push(parent_proc);
			}
		}
	}
}

// void Program::build_hoistable_defs(){
// 	for (auto p : proc_list)
// 	{
// 		p->gen_hoistable_defs();
// 	}
// 	bool active_list[proc_count];
// 	for (int i = 0; i<proc_count; i++){
// 		active_list[i] = true;
// 	}
// 	int num_active = proc_count;
// 	while(num_active > 0){
// 		for (int i = 0; i < proc_count; i++)
// 		{
// 			if(active_list[i]){
// 				active_list[i] = false;
// 				num_active--;
// 				for (auto e : *(proc_list[i]->get_succ_procs()))
// 				{
// 					bool changed = proc_list[i]->add_hoistable_defs(e.get_callee_proc()->
// 										get_hoistable_defs(), e.get_call_site_label());
// 					if(changed){
// 						for (auto pred : *(proc_list[i]->get_pred_procs()))
// 						{
// 							int caller_id = pred.get_caller_proc()->get_proc_id();
// 							if (!active_list[caller_id])
// 							{
// 								active_list[caller_id] = true;
// 								num_active++;
// 							}
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
	
// }

// void Program::print_hoistable_defs(){
// 	for(auto p: proc_list){
// 		cout << "Procedure: " + p->get_proc_name() << endl;
// 		p->print_hoistable_defs();
// 		cout << endl;
// 	}
// }

// void Program::build_hoisting_region(){
// 	for (auto p : proc_list)
// 	{
// 		for(auto def: p->get_hoistable_defs()){
// 			int lab = def.get_def_label();
// 			if (hoisting_region_map.count(lab)==0)
// 			{
// 				hoisting_region_map[lab] = set<int>();
// 			}

// 			for (auto call_site : def.get_call_sites())
// 			{
// 				hoisting_region_map[lab].insert(call_site);
// 			}
// 		}
// 	}
// }

// void Program::print_hoisting_region(){
// 	for(auto p: proc_list){
// 		cout << "Procedure: " << p->get_proc_name() << endl;
// 		p->print_hoisting_region();
// 		cout << endl;
// 	}
// }

void Program::compute(){
	build_pred_procs();
	build_all_lhs_vars();
	build_defs();
	// build_hoistable_defs();
	// build_hoisting_region();
}


Graph * Program::get_cfg_of_proc(string name)
{
	for (auto p : proc_list)
	{	
		if (p->get_proc_name() == name)
			return p->get_proc_cfg();
	}
	CHECK_INPUT(false, "Callee procedure not found");
	return NULL;
}

void Program::add_label_and_node(int label, Node *n)
{
	CHECK_INPUT((global_label_to_node_map.count(label) == 0), "Duplicate label");
	global_label_to_node_map[label] = n;
}

Node * Program::get_node_from_label(int label)
{

	CHECK_INVARIANT(global_label_to_node_map.find(label) != global_label_to_node_map.end(), "Node label not found in the program");

	return global_label_to_node_map[label];
}

// set<int> Program::get_hoisting_region_from_label(int label){
// 	return hoisting_region_map[label];
// }

int Program::find_node_id_of_call_node(int label)
{
	Node * n = get_node_from_label(label);
	int n_id = n->get_node_id();
	
	return n_id;
}

Graph * Program::find_cfg_of_node_label(int label)
{
	Node * n = get_node_from_label(label);
	Graph * g = n->get_cfg_of_node();
	
	return g;
}
