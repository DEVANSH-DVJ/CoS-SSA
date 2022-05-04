using namespace std;
#include "common-headers.hh"

static int parse_opt (int key, char *arg, struct argp_state *state);
int yyparse();

	/* Program documentation. */
	static char doc[] = "Print Paths";

	/* A description of the arguments we accept. */
	static char args_doc[] = "";

	struct argp_option options[] = {
		{ "length", 'l', "NUM", 0, "Bound on the length of the traces (default 10)"},
		{ "print-program", 'p', 0 , 0, "Print the program (disabled by default)"},
		{ "call-depth", 'd', "NUM", 0, "Bound on the depth of recursion (default 3)"},
		{ "print-traces", 't', 0 , 0, "Print traces (disabled by default)"},
		{ "input-file", 'f', "NAME" , 0, "Input file name (default stdin)"},
		{ 0 }
		};

	struct arguments {
    	int l;
    	int d;
	string f;
	}; 

	/* Our argp parser. */
	static struct argp argp = { options, parse_opt, args_doc, doc };
	
static bool program_print = false;

extern FILE * yyin;

int main (int argc, char **argv)
{
	struct arguments arguments;

	arguments.l=10;
	arguments.d=3;
	arguments.f="stdin";

	argp_parse (&argp, argc, argv, 0, 0, &arguments);

	
	int length = arguments.l;
	int depth = arguments.d;
	string file = arguments.f;

	if (file != "stdin")
	{	const char * file_name = file.c_str();
		yyin = fopen(file_name,"r");
	}

	yyparse();

	if (program_print)
		program.print_program();

	program.compute();
	program.print_kill_def();
}

static int parse_opt (int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = (struct arguments*)state->input;

	switch (key)
	{
		case 'd': 
			arguments->d = atoi(arg);
			break;
		case 'f':
			arguments->f = string(arg);
			break;
		case 'l': 
			arguments->l = atoi(arg);
			break;
		case 'p': 
			program_print = true;
			break;
	}

	return 0;
}

#if 0

int main()
{
	//int V = 5;
	vector<int> path;
	Path p;
	
	Graph g;

	g.add_edge(Node(0), Node(1));
	g.add_edge(Node(0), Node(4));
	g.add_edge(Node(1), Node(2));
	g.add_edge(Node(1), Node(3));
	g.add_edge(Node(1), Node(4));
	g.add_edge(Node(2), Node(3));
	g.add_edge(Node(3), Node(4));
	g.add_edge(Node(3), Node(5));
	g.add_edge(Node(5), Node(1));
	g.print_graph(6);
	//g.print_paths(path,0,16);
	g.build_list_of_paths(p,Node(0),16);
	g.print_path_list();
	return 0;
}

#endif
