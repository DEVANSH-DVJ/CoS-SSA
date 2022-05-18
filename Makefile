
CPP = g++
YACC = bison
LEX = flex

TGT = cs_ssa

OBJ = main.o error.o
CFG_OBJ = cfg/cfg.scan.o cfg/cfg.tab.o

HEADERS = error.hh argparse.hh

all: $(TGT)

$(TGT): $(OBJ) $(CFG_OBJ)
	$(CPP) $(OBJ) $(CFG_OBJ) -o $(TGT) -ly -ll

main.o: main.cc $(HEADERS) cfg/cfg.tab.h
	$(CPP) -c main.cc

error.o: error.cc error.hh stacktrace.h
	$(CPP) -c error.cc

$(CFG_OBJ) cfg/cfg.tab.h: $(HEADERS)
	$(MAKE) -C cfg clean
	$(MAKE) -C cfg

clean:
	$(MAKE) -C cfg clean
	rm -f *.o *.output
	rm -f $(TGT)
