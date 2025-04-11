// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File main.cpp made by Nathan Guimaraes (334437) on 28/03/2025

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "parser.tab.hpp"
#include "symbol.h"

using namespace std;

int yyparse();
extern char *yytext;

extern int getLineNumber();
extern void initMe();
extern FILE *yyin;
string tokenName(int token);

int main(int argc, char **argv){

    if (argc < 2){
        fprintf(stderr, "call: ./etapa1 input.txt \n");
        exit(1);
    }
    if (0 == (yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "Cannot open file %s... \n", argv[1]);
        exit(2);
    }

    initMe();

    yyparse();

    fprintf(stderr,"The file had %d lines in total.\n", getLineNumber());

    printTable();

    return 0;
}