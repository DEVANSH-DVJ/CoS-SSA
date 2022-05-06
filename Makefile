
CPP=g++
TGT=cs_ssa
OBJ=main.o lex.yy.o y.tab.o
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

clean:
	rm -f $(TGT)
	rm -f *.o *.output
	rm -f lex.yy.c y.tab.*
