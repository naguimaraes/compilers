// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File main.cpp made by Nathan Guimaraes (334437) on 28/03/2025

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "tokens.h"
#include "symbol.h"

using namespace std;

int yylex();
extern char *yytext;

extern int isRunning();
extern int getLineNumber();
extern void initMe();
extern FILE *yyin;
string tokenName(int token);

int main(int argc, char **argv){

    if (argc < 2)
    {
        fprintf(stderr, "call: ./etapa1 input.txt \n");
        exit(1);
    }
    if (0 == (yyin = fopen(argv[1], "r")))
    {
        fprintf(stderr, "Cannot open file %s... \n", argv[1]);
        exit(2);
    }

    initMe();

    int token;

    fprintf(stderr,"Starting the scanner...\n");
    while(isRunning() && (token = yylex())){
        if(token != TOKEN_ERROR){
            fprintf(stderr,"Found the token \"%s\" with type: \"%s\"\n", yytext, tokenName(token).c_str());
        }
        else{
            fprintf(stderr, "Found an error token! Breaking...\n");
            break;
        }
    }
    fprintf(stderr,"Finishing the scanner...\n");
    fprintf(stderr,"The file had %d lines in total.\n", getLineNumber());

    printTable();

    return 0;
}