
CPP = g++
YACC = bison
LEX = flex

TGT = cs_ssa

OBJ = main.o error.o
CFG_OBJ = cfg/cfg.scan.o cfg/cfg.tab.o cfg/cfg_node.o cfg/cfg_edge.o
SSA_OBJ = ssa/ssa.scan.o ssa/ssa.tab.o ssa/ssa_node.o ssa/ssa_edge.o ssa/ssa_meta.o
DDG_OBJ = ddg/ddg_node.o ddg/ddg_edge.o

HEADERS = error.hh argparse.hh

all: $(TGT)

$(TGT): $(OBJ) $(CFG_OBJ) $(SSA_OBJ) $(DDG_OBJ)
	$(CPP) $(OBJ) $(CFG_OBJ) $(SSA_OBJ) $(DDG_OBJ) -o $(TGT) -ly -ll

main.o: main.cc error.hh argparse.hh cfg/cfg.tab.hh ssa/ssa.tab.hh
	$(CPP) -c main.cc

error.o: error.cc error.hh stacktrace.h
	$(CPP) -c error.cc

cfg/cfg.tab.cc cfg/cfg.tab.hh: cfg/cfg.y
	$(YACC) -o cfg/cfg.tab.cc -dv cfg/cfg.y

cfg/cfg.scan.cc: cfg/cfg.l cfg/cfg.tab.hh
	$(LEX) -o cfg/cfg.scan.cc --yylineno cfg/cfg.l

ssa/ssa.tab.cc ssa/ssa.tab.hh: ssa/ssa.y
	$(YACC) -o ssa/ssa.tab.cc -dv ssa/ssa.y

ssa/ssa.scan.cc: ssa/ssa.l ssa/ssa.tab.hh
	$(LEX) -o ssa/ssa.scan.cc --yylineno ssa/ssa.l

cfg/%.o: cfg/%.cc
	$(CPP) -c $< -o $@

ssa/%.o: ssa/%.cc
	$(CPP) -c $< -o $@

ddg/%.o: ddg/%.cc
	$(CPP) -c $< -o $@

clean:
	rm -f $(TGT)
	rm -f *.o *.output
	rm -f cfg/*.o cfg/*.output
	rm -f cfg/cfg.scan.* cfg/cfg.tab.*
	rm -f ssa/*.o ssa/*.output
	rm -f ssa/ssa.scan.* ssa/ssa.tab.*
	rm -f ddg/*.o ddg/*.output
	rm -f ddg/ddg.scan.* ddg/ddg.tab.*
