// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File main.cpp made by Nathan Guimaraes (334437)

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "parser.tab.hpp"
#include "symbol.hpp"
#include "ast.hpp"
#include "verifications.hpp"
#include "tac.hpp"
#include "asm.hpp"
#include "optimizer.hpp"

int yyparse();
extern char *yytext;

extern int getLineNumber();
extern void initMe();
extern FILE *yyin;
extern ASTNode* root;
extern int getSyntaxErrors();
string tokenName(int token);

int main(int argc, char **argv){
    if (argc < 7){
        fprintf(stderr, "Arguments missing.\n");
        fprintf(stderr, "Call: ./etapa6 <input_file> <symbol_table_output> <ast_output> <decompiled_output> <tac_output> <assembly_output>\n");
        exit(1); // Exit code 1 for missing arguments
    }
    
    if (0 == (yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "Input file %s not found.\n", argv[1]);
        exit(2); // Exit code 2 for input file not found
    }

    // Syntax analysis and AST generation
    initMe();
    yyparse(); 
    fprintf(stderr,"The file had %d lines in total.\n", getLineNumber());
    
    // Check for syntax errors
    int syntaxErrors = getSyntaxErrors();
    
    if (syntaxErrors == 0) {
        fprintf(stderr, "Syntax analysis completed successfully - no errors found.\n");
    }

    // Save AST structure to file
    std::ofstream astFile(argv[3]);
    if (!astFile.is_open()) {
        fprintf(stderr, "File %s could not be opened for writing.\n", argv[3]);
        exit(2); // Exit code 2 for file not found
    }
    printASTToFile(argv[3], root);
    astFile.close();
    fprintf(stderr, "- AST structure saved to file \"%s\".\n", argv[3]);
    
    // Save decompiled code to file
    std::ofstream outFile(argv[4]);
    if (!outFile.is_open()) {
        fprintf(stderr, "File %s could not be opened for writing.\n", argv[4]);
        exit(2); // Exit code 2 for file not found
    }
    decompileAST(outFile, root, 0);
    outFile.close();
    fprintf(stderr, "- Decompiled code saved to file \"%s\".\n", argv[4]);

    int semanticErrors = semanticVerification(root);
    
    if(syntaxErrors > 0) {
        fprintf(stderr, "Syntax analysis completed with %d error(s).\n", syntaxErrors);

        if(semanticErrors > 0) {
            fprintf(stderr, "Semantic analysis completed with %d error(s).\n", semanticErrors);
            exit(4); // Exit code 4 for semantic error
        }
        else{
            fprintf(stderr, "Semantic analysis completed successfully - no errors found.\n");
            fprintf(stderr, "Compilation ended.\n");
            exit(3); // Exit code 3 for syntax error
        }
    }

    if (semanticErrors > 0) {
        fprintf(stderr, "Semantic analysis completed with %d error(s).\nCompilation ended.\n", semanticErrors);
        exit(4); // Exit code 4 for semantic error 
    } else {

        // If no syntax or semantic errors, proceed with TAC generation
        fprintf(stderr, "\nSemantic analysis completed successfully - no errors found.\n");
        
        initTAC();
        TAC* originalTAC = generateTAC(root);
        if (originalTAC) {

            // Original flow, from TAC to ASM without optimization
            // Save original TAC to file first (before optimization)
            std::ofstream tacFile(argv[5]);
            if (!tacFile.is_open()) {
                fprintf(stderr, "File %s could not be opened for writing.\n", argv[5]);
                exit(2); // Exit code 2 for file not found
            }
            tacFile.close();
            printTACToFile(argv[5], originalTAC);
            fprintf(stderr, "- TAC list saved to file \"%s\".\n", argv[5]);
            
            // Save original symbol table to file (after TAC generation)
            std::ofstream symbolFile(argv[2]);
            if (!symbolFile.is_open()) {
                fprintf(stderr, "File %s could not be opened for writing.\n", argv[2]);
                exit(2); // Exit code 2 for file not found
            }
            symbolFile.close();
            printSymbolTableToFile(argv[2]);
            fprintf(stderr, "- Symbol table saved to file \"%s\".\n", argv[2]);

            // Generate assembly code using original TAC
            std::ofstream asmFile(argv[6]);
            if (!asmFile.is_open()) {
                fprintf(stderr, "File %s could not be opened for writing.\n", argv[6]);
                exit(2); // Exit code 2 for file not found
            }
            asmFile.close();
            generateASM(originalTAC, argv[6]);
            fprintf(stderr, "- Original assembly code saved to file \"%s\".\n", argv[6]);




            // Generate assembly code using optimized TAC
            // Apply TAC optimizations
            fprintf(stderr, "\n");
            std::string baseFilename = std::string(argv[5]);
            if (baseFilename.size() >= 4 && baseFilename.substr(baseFilename.size() - 4) == ".txt") {
                baseFilename = baseFilename.substr(0, baseFilename.size() - 4);
            }
            std::string optimizedTacFilename = baseFilename + "_optimized.txt";
            TAC* optimizedTac = optimizeTAC(originalTAC, optimizedTacFilename);
            if (optimizedTac) {
                fprintf(stderr, "- TAC optimization completed successfully and saved to \"%s\".\n", optimizedTacFilename.c_str());
            } else {
                fprintf(stderr, "- TAC optimization failed.\n");
            }

            // Save optimized symbol table to file (after TAC generation)
            baseFilename = std::string(argv[2]);
            if (baseFilename.size() >= 4 && baseFilename.substr(baseFilename.size() - 4) == ".txt") {
                baseFilename = baseFilename.substr(0, baseFilename.size() - 4);
            }
            std::string optimizedSymbolTableFilename = baseFilename + "_optimized.txt";
            std::ofstream optimizedSymbolFile(optimizedSymbolTableFilename);
            if (!optimizedSymbolFile.is_open()) {
                fprintf(stderr, "File %s could not be opened for writing.\n", optimizedSymbolTableFilename.c_str());
                exit(2); // Exit code 2 for file not found
            }
            optimizedSymbolFile.close();
            printSymbolTableToFile(optimizedSymbolTableFilename);
            fprintf(stderr, "- Optimized symbol table saved to file \"%s\".\n", optimizedSymbolTableFilename.c_str());

            // Generate assembly code using optimized TAC
            baseFilename = std::string(argv[6]);
            if (baseFilename.size() >= 1 && baseFilename.substr(baseFilename.size() - 2) == ".s") {
                baseFilename = baseFilename.substr(0, baseFilename.size() - 2);
            }
            std::string optimizedAsmFilename = baseFilename + "_optimized.s";
            std::ofstream optimizedAsmFile(optimizedAsmFilename);
            if (!optimizedAsmFile.is_open()) {
                fprintf(stderr, "File %s could not be opened for writing.\n", optimizedAsmFilename.c_str());
                exit(2); // Exit code 2 for file not found
            }
            optimizedAsmFile.close();
            generateASM(optimizedTac, optimizedAsmFilename);
            fprintf(stderr, "- Optimized assembly code saved to file \"%s\".\n", optimizedAsmFilename.c_str());
        } else {
            fprintf(stderr, "TAC generation failed.\n");
        }
    }

    return 0;
}