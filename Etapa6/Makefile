# Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
# Makefile originally made by Ian Kersz (338368)
# Later edited by Nathan Guimaraes (334437)

PROJECT = etapa6
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

.PHONY: test
test: $(PROJECT)
	@if [ -z "$(FILE)" ]; then \
		echo "Usage: make test FILE=<input_file>"; \
		echo "Example: make test FILE=tests/io.2025++1"; \
		exit 1; \
	fi
	@if [ ! -f "$(FILE)" ]; then \
		echo "Error: File '$(FILE)' not found."; \
		exit 1; \
	fi
	@mkdir -p output
	@BASENAME=$$(basename "$(FILE)" .2025++1); \
	./$(PROJECT) "$(FILE)" "output/$${BASENAME}_symbol_table.txt" "output/$${BASENAME}_AST.txt" "output/$${BASENAME}_decompiled.txt" "output/$${BASENAME}_TAC.txt" "output/$${BASENAME}.s"; \
	if [ $$? -eq 0 ]; then \
		echo "\nGenerating binary code:"; \
		gcc -o "output/$${BASENAME}.out" "output/$${BASENAME}.s"; \
		if [ $$? -eq 0 ]; then \
			echo "- Binary code saved at output/$${BASENAME}.out"; \
			echo "\nRunning the program:"; \
			./output/$${BASENAME}.out; \
			echo "- Test completed successfully for $$BASENAME."; \
		else \
			echo "- Error: Failed to generate binary code."; \
		fi \
	else \
		echo "- Error: Compilation failed."; \
	fi

OBJS = lex.yy.o parser.tab.o ast.o symbol.o verifications.o tac.o asm.o main.o
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
	@rm -rf $(PROJECT) lex.yy.cpp *.o .docker-build $(PROJECT).tgz *.output *.tgz *.tab.* *.xml *.html *.gv *_recompiled.txt output/*

.PHONY: tgz
tgz:
	@touch $(PROJECT).tgz
	@tar cvzf $(PROJECT).tgz Makefile ast.cpp ast.hpp main.cpp parser.ypp scanner.l symbol.cpp symbol.hpp verifications.cpp verifications.hpp tac.cpp tac.hpp asm.cpp asm.hpp relatorio.md output/* tests/*