%{
    // Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
    // Lexical scanner (scanner.l) made by Nathan Guimaraes (334437) on 28/03/2025

    #include <string>
    #include "symbol.hpp"
    #include "parser.tab.hpp"
    using namespace std;

    int lineNumber = 1;
    int getLineNumber(){
        return lineNumber;
    }

    void initMe(){
        
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

\"([^\n"]|(\\\"))*\"        {yylval.symbol = insertSymbol(yytext, LIT_STRING, lineNumber); return LIT_STRING;}
\'.\'                       {yylval.symbol = insertSymbol(yytext, LIT_CHAR, lineNumber); return LIT_CHAR;}
[0-9]+\/[0-9]+              {yylval.symbol = insertSymbol(invertNumberReal(yytext), LIT_REAL, lineNumber); return LIT_REAL;}
[0-9]+                      {yylval.symbol = insertSymbol(invertNumberInt(yytext), LIT_INT, lineNumber); return LIT_INT;}
[a-zA-Z\_][a-zA-Z0-9\_]*    {yylval.symbol = insertSymbol(yytext, TK_IDENTIFIER, lineNumber); return TK_IDENTIFIER;}

[ \t]+                      {;}
\n                          {lineNumber++;}
.                           {return YYerror;}

<COMMENT>"--/"              {BEGIN(INITIAL);}
<COMMENT>\n                 {lineNumber++;}
<COMMENT>.                  {;}


%%

int yywrap(){
    return 1;
}
