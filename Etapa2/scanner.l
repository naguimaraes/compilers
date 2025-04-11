%{
    // Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
    // Lexical scanner (scanner.l) made by Nathan Guimaraes (334437) on 28/03/2025

    #include "symbol.h"
    #include "parser.tab.hpp"
    #include <string>
    using namespace std;

    int lineNumber = 1;
    int getLineNumber(){
        return lineNumber;
    }

    void initMe(){
        
    }

    string invertNumberInt(string number){
        
        string invertedNum(number.size(), '\0');

        for(unsigned long i = 0; i < number.size(); i++){
            invertedNum[i] = number[number.size() - i - 1];
        }

        return invertedNum;
    }

    string invertNumberReal(string number){
        unsigned long i = 0;

        while(number[i] != '/'){
            i++;
        }

        return invertNumberInt(number.substr(0, i)) + "/" + invertNumberInt(number.substr(i + 1, number.size() - i - 1));        
    }

%}

%option nounput noinput
%x COMMENT 

%%

"byte"                      {return KW_BYTE;}
"int"                       {return KW_INT;}
"real"                      {return KW_REAL;}
"if"                        {return KW_IF;}
"else"                      {return KW_ELSE;}
"do"                        {return KW_DO;}
"while"                     {return KW_WHILE;}
"read"                      {return KW_READ;}
"print"                     {return KW_PRINT;}
"return"                    {return KW_RETURN;}

"<="                        {return OPERATOR_LE;}
">="                        {return OPERATOR_GE;}
"=="                        {return OPERATOR_EQ;}
"!="                        {return OPERATOR_DIF;}

"/--"                       {BEGIN(COMMENT);}
"//".*                      {;}
[\-\,\;\:\(\)\[\]\{\}]      {return yytext[0];}
[\=\+\*\/\%\<\>\&\|\~]      {return yytext[0];}

\"([^\n"]|(\\\"))*\"        {insertSymbol(yytext, LIT_STRING); return LIT_STRING;}
\'.\'                       {insertSymbol(yytext, LIT_CHAR); return LIT_CHAR;}
[0-9]+\/[0-9]+              {insertSymbol(invertNumberReal(yytext), LIT_REAL); return LIT_REAL;}
[0-9]+                      {insertSymbol(invertNumberInt(yytext), LIT_INT); return LIT_INT;}
[a-zA-Z\_][a-zA-Z0-9\_]*    {insertSymbol(yytext, TK_IDENTIFIER); return TK_IDENTIFIER;}

[ \t]+                      {;}
\n                          {lineNumber++;}
.                           {return TOKEN_ERROR;}

<COMMENT>"--/"              {BEGIN(INITIAL);}
<COMMENT>\n                 {lineNumber++;}
<COMMENT>.                  {;}


%%

int yywrap(){
    return 1;
}
