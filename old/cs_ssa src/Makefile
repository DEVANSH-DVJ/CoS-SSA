HEADERS = error-display.hh graph.hh common-headers.hh 

OBJ = main.o graph.o error-display.o $(SCAN).o $(PARSE).tab.o

CPP = g++
BISON = bison
FLEX = flex
TGT = cs_ssa
SCAN = scan
PARSE = parse


CFLAGS = -g -rdynamic -fpermissive


$(TGT): $(OBJ) 
	$(CPP) --std=c++0x $(CFLAGS) $(OBJ) -o $(TGT) -ly
	strip $(TGT)

# removed -lfl option from the above command and added %option noyywrap in $(SCAN).l file.

$(SCAN).o: $(SCAN).c $(HEADERS)
	$(CPP) --std=c++0x  $(CFLAGS) -c $<

$(PARSE).tab.o:$(PARSE).tab.c $(HEADERS)
	$(CPP) --std=c++0x  $(CFLAGS) -c -fpermissive $<

main.o: main.cc $(HEADERS)
	$(CPP) --std=c++0x  $(CFLAGS) $(VERSION) -c main.cc

%.o: %.cc $(HEADERS)
	$(CPP) --std=c++0x  $(CFLAGS) -c $<

%.o: %.cxx $(HEADERS)
	$(CPP) --std=c++14  $(CFLAGS) -c $<

$(SCAN).c : $(SCAN).l $(PARSE).tab.h
	$(FLEX) -l --yylineno -o $(SCAN).c $(SCAN).l 

$(PARSE).tab.c $(PARSE).tab.h : $(PARSE).y
	$(BISON) -dv -b $(PARSE) $(PARSE).y
clean :
	rm -f *.o *.gch
	rm -f $(TGT) *.output *.spim *.toks *.ast *.sym *.icode *.tac *.rtl *.log 
	rm -rf $(PARSE).tab.c lex.yy.c $(PARSE).tab.h $(SCAN).c
	rm -rf html latex
