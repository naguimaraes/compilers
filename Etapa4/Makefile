# Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
# Makefile originally made by Ian Kersz (338368)
# Later edited by Nathan Guimaraes (334437)

PROJECT = etapa5
DEFAULT_VERSION = debug

CXX = g++
STD = -std=c++11
CXXFLAGS = -Wall -Wextra -pedantic -Wconversion $(STD)
RELEASE_FLAGS = -O2
DEBUG_FLAGS = -g -DDEBUG
current_dir = /$(shell pwd)
DEP_FILES = parser.tab.hpp

LEX = flex
BISON = bison
BISONFLAGS = -d -g -v --html

.PHONY: all
all: $(DEFAULT_VERSION)

.PHONY: release
release: CXXFLAGS += $(RELEASE_FLAGS)
release: target

.PHONY: debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: target

.PHONY: target
target: $(PROJECT)

.PHONY:
run: $(PROJECT)
	@./$(PROJECT) $(PROJECT).txt $(PROJECT)_decompiled.txt

test: $(PROJECT)
	@./$(PROJECT) $(PROJECT).txt $(PROJECT)_recompiled.txt 
	@./$(PROJECT) $(PROJECT)_recompiled.txt  $(PROJECT)_recompiled_recompiled.txt 
	@diff $(PROJECT)_recompiled.txt $(PROJECT)_recompiled_recompiled.txt > /dev/null && echo "\n2 times recompiled code is equal to 1 time recompiled code." || echo "\nRecompiling failed."

OBJS = lex.yy.o parser.tab.o ast.o symbol.o verifications.o main.o
$(PROJECT): $(OBJS)
	$(CXX) $(OBJS) -o $(PROJECT)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) $< -c

lex.yy.cpp: scanner.l $(DEP_FILES) parser.tab.hpp
	$(LEX) -o lex.yy.cpp scanner.l 

parser.tab.cpp parser.tab.hpp: parser.ypp
	$(BISON) $(BISONFLAGS) parser.ypp

.PHONY: versions
versions:
	$(CXX) --version | grep version && $(LEX) --version && $(BISON) --version | grep bison && $(MAKE) --version | grep GNU

.PHONY: clean
clean:
	@rm -f $(PROJECT) lex.yy.cpp *.o .docker-build $(PROJECT).tgz *.output *.tgz *.tab.* *.xml *.html *.gv *_recompiled.txt

.PHONY: tgz
tgz: clean
	@touch $(PROJECT).tgz
	@tar cvzf $(PROJECT).tgz --add-file=Makefile --add-file=ast.cpp --add-file=ast.hpp --add-file=main.cpp --add-file=parser.ypp --add-file=scanner.l --add-file=symbol.cpp --add-file=symbol.hpp --add-file=$(PROJECT).txt --add-file=verifications.cpp --add-file=verifications.hpp