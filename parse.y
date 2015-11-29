%{
#include <iostream>
#include "ast.h"
extern int yylex();
void yyerror(const char * msg) { std::cout << msg << std::endl; }
%}

%union {
  struct ast *a;
  double d;
  int tok;
  struct symbol *s;
  struct paramlist_formal *paramlist_form;
  struct paramlist_actual *paramlist_act;
}

%token CR
%token ASSIGN
%token PRINT
%token IDENT 
%token NUMBER
%token FLOAT
%token LPAREN
%token RPAREN
%token DEF
%token COLON
%token COMMA
%token PASS
%token IF
%token END
%token RETURN
%token PLUS
%token MINUS
%token MULT
%token DIV
%token MOD
%token DOUBLE_MULT
%token LESS
%token LESSEQ
%token GREAT
%token GREATEQ
%token EQ
%token NOTEQ

%left  PLUS MINUS
%left  MULT DIV MOD
%left  DOUBLE_MULT
%left  LESS LESSEQ GREAT GREATEQ EQ NOTEQ

%nonassoc UMINUS
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%type <a> stmt selection line expr
%type <paramlist_form> param_list_formal
%type <paramlist_act> param_list_actual
%type <d> FLOAT NUMBER
%type <s> IDENT
%type <tok> PLUS MINUS MULT DIV MOD DOUBLE_MULT LESS LESSEQ GREAT GREATEQ EQ NOTEQ
%%

start   : start funcdef	{;}
        | funcdef { ; }
        ;

funcdef : DEF IDENT LPAREN param_list_formal RPAREN COLON stmt END	{dodef($2, $4, $7);}
		| CR {;}
        ;

param_list_actual 
        : expr							{ $$ = newparamlist_actual($1, NULL); }
        | expr COMMA param_list_actual	{ $$ = newparamlist_actual($1, $3); }
        | 								{ $$ = NULL; }
        ;

param_list_formal   
		: IDENT							{ $$ = newparamlist_formal($1, NULL); }
		| IDENT COMMA param_list_formal	{ $$ = newparamlist_formal($1, $3); }
		| 								{ $$ = NULL; }
		;

stmt    : CR {$$ = NULL;}
		| PASS CR {$$ = NULL;}
		| line CR { $$ = newast('L', NULL, $1);}
		| selection CR{ $$ = newast('L', NULL, $1); }
		| stmt CR {$$ = $1;}
		| stmt PASS CR {$$ = $1;}
		| stmt line	CR{ $$ = newast('L', $1, $2); }
        | stmt selection CR{ $$ = newast('L', $1, $2); }
        ;

selection 
        : IF expr COLON stmt %prec LOWER_THAN_ELSE END	{ $$ = newflow('I', $2, $4, NULL); }
	    | IF expr COLON stmt ELSE COLON stmt END		{ $$ = newflow('I', $2, $4, $7); }
        ;

line    : IDENT ASSIGN expr { $$ = newasgn($1, $3); }
        | IDENT LPAREN param_list_actual RPAREN { $$ = newcall($1, $3); }
        | PRINT expr		{ $$ = newast('P', NULL, $2); }
        | RETURN expr { $$ = newast('R', NULL, $2); }
        ;

expr    : expr PLUS expr	{ $$ = newast($2, $1,$3); }
        | expr MINUS expr	{ $$ = newast($2, $1,$3); }
        | expr MULT expr	{ $$ = newast($2, $1,$3); }
        | expr DOUBLE_MULT expr	{ $$ = newast($2, $1,$3); }
        | expr DIV expr		{ $$ = newast($2, $1,$3); }
        | expr MOD expr		{ $$ = newast($2, $1,$3); }
        | expr LESS expr	{ $$ = newast($2, $1,$3); }
        | expr LESSEQ expr	{ $$ = newast($2, $1,$3); }
        | expr GREAT expr	{ $$ = newast($2, $1,$3); }
        | expr GREATEQ expr	{ $$ = newast($2, $1,$3); }
        | expr NOTEQ expr	{ $$ = newast($2, $1,$3); }
        | expr EQ expr		{ $$ = newast($2, $1,$3); }
        | MINUS expr %prec UMINUS	{ $$ = newast('M', NULL, $2); }
        | NUMBER	{ $$ = newnum($1); }
        | FLOAT		{ $$ = newnum($1); }
        | IDENT		{ $$ = newref($1); }
		| IDENT LPAREN param_list_actual RPAREN { $$ = newcall($1, $3);}
        ;
%%
