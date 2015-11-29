%{
#include <iostream>
#include <cstring>
#include "parse.tab.h"
struct symbol* lookup(char* s);
%}
letter      [a-zA-Z]
ident       {letter}({letter}|{digit})*
digit       [0-9]
digits      [0-9]+
float       {digits}"."{digits}

extern      yylval
%%

"**"       { yylval.tok = DOUBLE_MULT; return DOUBLE_MULT; }
"="        { return ASSIGN; }
"+"        { yylval.tok = PLUS; return PLUS; }
"-"        { yylval.tok = MINUS; return MINUS; }
"*"        { yylval.tok = MULT; return MULT; }
"/"        { yylval.tok = DIV; return DIV; }
"%"        { yylval.tok = MOD; return MOD; }
"("        { return LPAREN; }
")"        { return RPAREN; }
":"        { return COLON; }
","        { return COMMA; }
"<"        { yylval.tok = LESS; return LESS; }
">"        { yylval.tok = GREAT; return GREAT; }
"<="       { yylval.tok = LESSEQ; return LESSEQ; }
">="       { yylval.tok = GREATEQ; return GREATEQ; }
"=="       { yylval.tok = EQ; return EQ; }
"!="       { yylval.tok = NOTEQ; return NOTEQ; }
"if"       { return IF; }
"else"     { return ELSE; }
"def"      { return DEF; }
"pass"     { return PASS; }
"print"    { return PRINT; }
"end"      { return END; }
"return"   { return RETURN; }

{ident}    {
             yylval.s = lookup(yytext); return IDENT; 
           }

{float}    { 
             yylval.d = atof(yytext); return FLOAT; 
           }
{digits}   { 
             yylval.d = atof(yytext); return NUMBER; 
           }
"\n"       { return CR; }
.          { ; }

%%
int yywrap() {
  yylex_destroy();
  return 1;
}
