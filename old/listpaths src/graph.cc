#include "common-headers.hh"


int Node::get_node_id()
{
	return node_id;
}

void Node::set_node_id(int i)
{
	node_id = i;
}

int Node::get_node_label()
{
	return label;
}

StmtType Node::get_node_type()
{
	return node_type;
}

string Node::get_callee_name()
{
	CHECK_INVARIANT (node_type == call_stmt, "Call name is defined only for a call statement");

	return lhs;
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
}

void Graph::add_edge(int l1, int l2)
{
	int id1 = get_node_from_label(l1)->get_node_id();
	int id2 = get_node_from_label(l2)->get_node_id();

	CHECK_INPUT_AND_ABORT (l1 > 0, "Node label must be greater than 0");
	CHECK_INPUT_AND_ABORT (l2 > 0, "Node label must be greater than 0");
	succ[id1].push_back(id2);
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

void Graph::build_list_of_paths(Path path, int n_id, int length, Callstring cs, int depth)
{
	Node * n = get_node_from_id(n_id);
	int node_label = n->get_node_label();

	if ((length == 1) || ((proc_name == "main") && (succ[n_id].size() == 0)))
	{
		path.add_cs_node(cs,node_label);
		program.add_path_to_path_list(path);
	}
	else
	{
		// It is best to set call string as a vector if call nodes along with each n_id in a path 
		// rather than associating it with a node because we are storing n_id which will be same
		// for each instance of the node.
		//
		// If this is a call node, we need to 
		// 	(a) skip including this node in the path
		// 	(b) suffix the call node to current call string 
		// 	(c) start including the nodes from other procedure.
		// Does it make sense to lift this procedure to a Program where as have a global view?
		// An alternative is to store a global view and make it accessible within this procedure
		

		if ((n->get_node_type() == call_stmt) && (cs.recursion_depth() <= depth))
		{ 	
			Callstring new_cs = cs.append_call_node(node_label);
			
			// Get the callees cfg and start building it's paths
			
			string callee_name = n->get_callee_name();
			Graph * callee_cfg = program.get_cfg_of_proc(callee_name);

			callee_cfg->build_list_of_paths(path,0,length,new_cs, depth);
		}
		else 
		{	
			// Either it's call node and depth has saturated or it's a non-call node.
			// In either case, we add the node to the path and proceed to its successors.
			
			path.add_cs_node(cs,node_label);
			if (succ[n_id].size() > 0) 
			{
				for (auto s : succ[n_id])
				{
					build_list_of_paths(path,s,length-1,cs, depth);
				}
			}
			else	 
			{		
				// Since the node has no successor, we have reached the end of the procedure.
				// In such a situation, we need to
   				//	- remove the last call node from the call string
   				//	- find the cfg of the call node
   				//	- find the node id of the call node
   				//	- restart the build_path list on the cfg with the
      				//		. new path
      				//		. new call string
      				//		. successors of the call node in the cfg
	
	
      				int last_call_node_label = cs.get_last_call_node();
				Node * n = program.get_node_from_label(last_call_node_label);
				Graph * caller_cfg = n->get_cfg_of_node();
				int last_call_node_id = n->get_node_id();
	
				//cout << "At the End of " << proc_name << " last call node is " << last_call_node_label << endl;
				//cout << "The successors of the last call node are " << endl;
	
				Callstring new_cs = cs.remove_last_call_node();
	
				for (auto s : get_succ_vector(last_call_node_id))
				{
					//cout << (caller_cfg->get_node_from_id(s))->get_node_label() << endl;
					caller_cfg->build_list_of_paths(path,s,length-1,new_cs, depth);
				}
			}
		}

#if 0
		Old code. It was creating paths by abstracting out the callee paths too.

		if ((n->get_node_type() == call_stmt) && (cs.size() < depth))
		{
			Callstring new_cs = cs.append_call_node(node_label);
			
			// Get the callees cfg and start building it's paths
			
			string callee_name = n->get_callee_name();
			Graph * callee_cfg = program.get_cfg_of_proc(callee_name);

			callee_cfg->build_list_of_paths(path,0,length,new_cs, depth);

		}
		else	// Only a non-call node is added to the path
		{
			path.add_cs_node(cs,node_label);
		}
		
		// When the callee's paths are over, we need to continue with the paths of the current procedure
			
		if (succ[n_id].size() > 0) 
		{
			for (auto s : succ[n_id])
			{
				build_list_of_paths(path,s,length-1,cs, depth);
			}
		}
		else 
		{	
			// When we reach the end of a procedure, we need to
   			//	- remove the last call node from the call string
   			//	- find the cfg of the call node
   			//	- find the node id of the call node
   			//	- restart the build_path list on the cfg with the
      			//		. new path
      			//		. new call string
      			//		. successors of the call node in the cfg


      			int last_call_node_label = cs.get_last_call_node();
			Node * n = program.get_node_from_label(last_call_node_label);
			Graph * caller_cfg = n->get_cfg_of_node();
			int last_call_node_id = n->get_node_id();

			//cout << "At the End of " << proc_name << " last call node is " << last_call_node_label << endl;
			//cout << "The successors of the last call node are " << endl;

			Callstring new_cs = cs.remove_last_call_node();

			for (auto s : get_succ_vector(last_call_node_id))
			{
				//cout << (caller_cfg->get_node_from_id(s))->get_node_label() << endl;
				caller_cfg->build_list_of_paths(path,s,length-1,new_cs, depth);
			}
		}
#endif
	}
}


void Path::print_path_using_node_labels()
{
	for (auto p : path)
	{
		Callstring cs = p.first;
		int label = p.second;
		Node * n = program.get_node_from_label(label);
		cs.print_callstring();
		n->print_node();
		cout << endl;
	}
	cout << endl;
}

void Path::add_cs_node(Callstring cs, int n_id)
{
	path.push_back(make_pair(cs,n_id));
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


#if 0
void Procedure::print_path_list(Callstring cs, int length, int depth)
{
	Path temp;
	cfg->build_list_of_paths(temp, 0 , length, cs, depth);
	cfg->print_path_list(length, depth);
}
#endif

void Callstring::print_callstring()
{
	int s = callstring.size();

	for (int i=0; i<s; i++)
		cout << "  ";

	cout << "[";
	for (int i=0; i<s; i++)
	{
		if (i==0)
		{
			CHECK_INVARIANT (callstring[i]==0, "First call site must be 0");
		}
		else if (i==1)
			cout << callstring[i];
		else
			cout << "." << callstring[i];
	}
	cout << "] ";
}

int Callstring::recursion_depth()
{
	int s = callstring.size();
	int maxcount = 0;

	for (int i=0; i<s; i++)
	{
		int node = callstring[i];
		int count = 1;
		for (int j=i+1; j<s; j++)
		{
			if (callstring[j] == node)
			{
				count++;
				maxcount = count > maxcount? count: maxcount;
			}
		}
	}
	return maxcount;
} 

Callstring Callstring::append_call_node(int n) 
{ 
	vector <int> temp = callstring;
	temp.push_back(n); 

	Callstring new_callstring (temp);
	return new_callstring;
}

Callstring Callstring::remove_last_call_node() 
{ 
	vector <int> temp = callstring;

	CHECK_INVARIANT (!temp.empty(), "Attempt to remove call node from an empty call string");
	temp.pop_back(); 

	Callstring new_callstring (temp);
	return new_callstring;
}

int Callstring::get_last_call_node()
{
	CHECK_INVARIANT (!callstring.empty(), "Attempt to find last call node in an empty call string");
	return callstring.back();
}

string Opd::get_opd_string()
{
	if (opd_type == num_opd)
		return to_string(value);
	else if (opd_type == var_opd)
		return name;
	else
		CHECK_INVARIANT(false, "Wrong Opd Type");
}

void Program::add_procedure(Procedure *p)
{
	proc_list.push_back(p);
}

void Program::print_program()
{
	for (auto p : proc_list)
		p->print_cfg();
}

void Program::build_path_list(int length, int depth)
{

	bool found_main = false;

	for (auto p : proc_list)
	{	if (p->is_main_proc())
		{
			found_main = true;
			Callstring cs(0);
			Path temp;

			Graph * cfg_of_main = p->get_proc_cfg(); 

			// The procedure below will add all paths to the path_list within main
			cfg_of_main->build_list_of_paths(temp, 0 , length, cs, depth);
		}
	}
	CHECK_INPUT(found_main, "No main procedure found");
}

Graph * Program::get_cfg_of_proc(string name)
{
	for (auto p : proc_list)
	{	
		if (p->get_proc_name() == name)
			return p->get_proc_cfg();
	}
	CHECK_INPUT(false, "Callee procedure not found");
}

void Program::add_label_and_node(int label, Node *n)
{
	CHECK_INPUT((global_label_to_node_map.count(label) == 0), "Duplicate label");
	global_label_to_node_map[label] = n;
}

void Program::add_path_to_path_list(Path path)
{
	path_list.push_back(path);
}

void Program::print_path_list(int length, int depth)
{
	int count=0;

	cout << "---------------------------------------------------" << endl;
	cout << "Listing all traces upto length=" << length << " and call depth=" << depth << endl;
	cout << "Each line in a trace has the format \"[call_string] node_number: statement\" " << endl;
	cout << "---------------------------------------------------" << endl;

	for (auto p : path_list)
	{
		cout << "Path #" << ++count << endl;
		p.print_path_using_node_labels();
	}
}

Node * Program::get_node_from_label(int label)
{

	CHECK_INVARIANT(global_label_to_node_map.find(label) != global_label_to_node_map.end(), "Node label not found in the program");

	return global_label_to_node_map[label];
}

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
