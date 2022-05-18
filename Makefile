
CPP = g++
YACC = bison
LEX = flex

TGT = cs_ssa

OBJ = main.o error.o
CFG_OBJ = cfg/cfg.scan.o cfg/cfg.tab.o
CFG_SRC = cfg/cfg.y cfg/cfg.l

HEADERS = error.hh argparse.hh

all: $(TGT)

$(TGT): $(OBJ) $(CFG_OBJ)
	$(CPP) $(OBJ) $(CFG_OBJ) -o $(TGT) -ly -ll

main.o: main.cc $(HEADERS) cfg/cfg.tab.h
	$(CPP) -c main.cc

error.o: error.cc error.hh stacktrace.h
	$(CPP) -c error.cc

cfg/cfg.tab.o cfg/cfg.tab.h: cfg/cfg.y
	$(YACC) -b cfg/cfg -dv cfg/cfg.y
	$(CPP) -c cfg/cfg.tab.c -o cfg/cfg.tab.o

cfg/cfg.scan.o: cfg/cfg.l cfg/cfg.tab.h
	$(LEX) --yylineno --outfile=cfg/cfg.scan.c cfg/cfg.l
	$(CPP) -c cfg/cfg.scan.c -o cfg/cfg.scan.o

clean:
	rm -f $(TGT)
	rm -f *.o *.output
	rm -f cfg/*.o cfg/*.output
	rm -f cfg/cfg.scan.* cfg/cfg.tab.*
