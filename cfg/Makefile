
CPP = g++
YACC = bison
LEX = flex

OBJ = cfg.scan.o cfg.tab.o

all: $(OBJ)

cfg.tab.o cfg.tab.h: cfg.y
	$(YACC) -b cfg -dv cfg.y
	$(CPP) -c cfg.tab.c

cfg.scan.o: cfg.l cfg.tab.h
	$(LEX) --yylineno --outfile=cfg.scan.c cfg.l
	$(CPP) -c cfg.scan.c

clean:
	rm -f *.o *.output
	rm -f cfg.scan.* cfg.tab.*
