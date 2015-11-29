#include <string.h>
#include <stdlib.h>
#include "parse.tab.h"
#include <map>

/* symbol table */
struct symbol {		
  char *name;	/* variable/function name */
  double value;	/* variable value */
  struct ast *stmt;	/* stmt for the function */
  struct paramlist_formal *paramlist_form; /* formal parameter list*/
};  

/* actual parameter list */
struct paramlist_actual {
  struct ast *parm;
  struct paramlist_actual *next;
};

/* formal parameter list */
struct paramlist_formal {
  struct symbol *sym;
  struct paramlist_formal *next;
};

struct ast {
  int nodetype;
  struct ast *l;
  struct ast *r;
};

struct numval {
  int nodetype;			/* type K */
  double number;
};

struct symref {
  int nodetype;			/* type N */
  struct symbol *s;
};

struct symasgn {
  int nodetype;			/* type = */
  struct symbol *s;
  struct ast *v;		/* value */
};

struct ufncall {		/* user function */
  int nodetype;			/* type C */
  struct paramlist_actual *l;		/* list of arguments */
  struct symbol *s;
};

struct flow {
  int nodetype;			/* type I */
  struct ast *cond;		/* condition */
  struct ast *tl;		/* then or do list */
  struct ast *el;		/* optional else list */
};

/* lookup symbol table */
struct symbol* lookup(char* s);
struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newref(struct symbol *s);
struct ast *newasgn(struct symbol *s, struct ast *v);
struct ast *newnum(double d);
struct ast *newcall(struct symbol *s, struct paramlist_actual *l);
struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el);

struct paramlist_actual* newparamlist_actual(struct ast *parm, struct paramlist_actual *next);
struct paramlist_formal* newparamlist_formal(struct symbol *sym, struct paramlist_formal *next);

/* define a function */
ast* dodef(struct symbol *name, struct paramlist_formal *paramlist_form, struct ast *stmt);

void yyerror(const char * msg);

void evaluate(void);

double eval(struct ast *a);

double calluser(struct ufncall *f);

void free_mem(void);

void ast_free(struct ast *a);

void free_paramlist_actual(struct paramlist_actual* paramlist_act);

void free_paramlist_formal(struct paramlist_formal* paramlist_form);

