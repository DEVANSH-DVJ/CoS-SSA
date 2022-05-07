
CPP=g++
TGT=cs_ssa
OBJ=main.o lex.yy.o y.tab.o error.o program.o procedure.o edge.o node.o opd.o
YACC=bison
LEX=flex

all: $(TGT)

$(TGT): $(OBJ)
	$(CPP) $(OBJ) -o $(TGT) -ly -ll

main.o: main.cc argparse.hh y.tab.h
	$(CPP) -c main.cc

y.tab.o y.tab.h: parse.y
	$(YACC) -b y -dv parse.y
	$(CPP) -c y.tab.c

lex.yy.o: scan.l y.tab.h
	$(LEX) --yylineno scan.l
	$(CPP) -c lex.yy.c

error.o: error.cc error.hh
	$(CPP) -c error.cc

program.o: program.cc program.hh procedure.o
	$(CPP) -c program.cc

procedure.o: procedure.cc procedure.hh node.o
	$(CPP) -c procedure.cc

edge.o: edge.cc edge.hh node.o
	$(CPP) -c edge.cc

node.o: node.cc node.hh opd.o
	$(CPP) -c node.cc

opd.o: opd.cc opd.hh
	$(CPP) -c opd.cc

clean:
	rm -f $(TGT)
	rm -f *.o *.output
	rm -f lex.yy.c y.tab.*
