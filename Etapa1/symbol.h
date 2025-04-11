// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File symbol.h made by Nathan Guimaraes (334437) on 28/03/2025

#include <string>

using namespace std;

class Symbol {
    private:
        int type;
        string lex;
    public:
        Symbol(string lex, int type) {
            this->type = type;
            this->lex = lex;
        }
    
        string getLex() {
            return lex;
        }
    
        int getType() {
            return type;
        }
};

Symbol *insertSymbol(string lex, int type);

void printTable();
