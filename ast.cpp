#include "ast.h"
#include "math.h"
#include <iostream>
#include <stdio.h>

std::map<std::string, struct symbol> symtab;
struct symbol* lookup(char* s){
	std::map<std::string, struct symbol>::iterator pos = symtab.find(s);
	if(pos!=symtab.end()) return &(pos->second);

	struct symbol sym;
	
	sym.name = (char*)malloc(strlen(s)+1);
	strcpy(sym.name, s);
	sym.value = 0;
	sym.stmt = NULL;
	sym.paramlist_form = NULL;
	symtab[s] = sym;
	return &symtab[s];
}

struct ast * newast(int nodetype, struct ast *l, struct ast *r)
{
  struct ast *a = (ast *)malloc(sizeof(struct ast));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = nodetype;
  a->l = l;
  a->r = r;
  return a;
}

struct ast * newnum(double d)
{

  struct numval *a = (numval *)malloc(sizeof(struct numval));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = 'K';
  a->number = d;
  
  return (struct ast *)a;
}

struct ast * newref(struct symbol *s)
{

  struct symref *a = (symref *)malloc(sizeof(struct symref));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = 'N';
  a->s = s;
  
  return (struct ast *)a;
}

struct ast * newcall(struct symbol *s, struct paramlist_actual *l)
{

  struct ufncall *a = (ufncall *)malloc(sizeof(struct ufncall));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = 'C';
  a->l = l;
  a->s = s;
  return (struct ast *)a;
}

struct ast * newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el)
{

  struct flow *a = (flow *)malloc(sizeof(struct flow));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = nodetype;
  a->cond = cond;
  a->tl = tl;
  a->el = el;
  return (struct ast *)a;
}

struct ast * newasgn(struct symbol *s, struct ast *v)
{

  struct symasgn *a = (symasgn *)malloc(sizeof(struct symasgn));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = '=';
  a->s = s;
  a->v = v;
  
  return (struct ast *)a;
}


struct paramlist_actual* newparamlist_actual(struct ast *param, struct paramlist_actual* next)
{

  struct paramlist_actual* paramlist = (paramlist_actual*)malloc(sizeof(struct paramlist_actual));
  
  if(!paramlist) {
    yyerror("out of space");
    exit(0);
  }
  paramlist->parm = param;
  paramlist->next = next;
  return paramlist;
}

struct paramlist_formal* newparamlist_formal(symbol *param, struct paramlist_formal* next)
{

  struct paramlist_formal* paramlist_form = (paramlist_formal*)malloc(sizeof(struct paramlist_formal));
  
  if(!paramlist_form) {
    yyerror("out of space");
    exit(0);
  }
  paramlist_form->sym = param;
  paramlist_form->next = next;
  return paramlist_form;
}


void free_paramlist_actual(struct paramlist_actual* paramlist_act)
{
  struct paramlist_actual *nparms;

  while(paramlist_act) {
    nparms = paramlist_act->next;
	ast_free(paramlist_act->parm);
    free(paramlist_act);
    paramlist_act = nparms;
  }
}

void free_paramlist_formal(struct paramlist_formal* paramlist_form)
{
  struct paramlist_formal *nparms;

  while(paramlist_form) {
    nparms = paramlist_form->next;
    free(paramlist_form);
    paramlist_form = nparms;
  }
}


/* define a function */
ast* dodef(struct symbol *sym, struct paramlist_formal *paramlist_form, struct ast *stmt)
{
  if( (sym->paramlist_form == NULL) && (sym->stmt == NULL)){
  	sym->paramlist_form = paramlist_form;
  	sym->stmt = stmt;
  	return sym->stmt;
  	}
  else {
  	char s[100];
	sprintf(s, "Duplicate function definition %s", sym->name);
  	yyerror(s);
	ast_free(stmt);
	free_paramlist_formal(paramlist_form);
	free_mem();
	exit(0);
  	}
}

double eval(struct ast *a)
{
  double v;

  if(!a) {
    return 0.0;
  }
  
  switch(a->nodetype) {
    /* constant */
  case 'K': v = ((struct numval *)a)->number; break;

    /* name reference */
  case 'N': v = ((struct symref *)a)->s->value; break;

    /* assignment */
  case '=': ((struct symasgn *)a)->s->value =
      eval(((struct symasgn *)a)->v); break;

    /* expressions */
  case PLUS: v = eval(a->l) + eval(a->r); break;
  case MINUS: v = eval(a->l) - eval(a->r); break;
  case MULT: v = eval(a->l) * eval(a->r); break;
  case DIV: v = eval(a->l) / eval(a->r); break;
  case MOD: v = (int)eval(a->l)%(int)eval(a->r); break;
  case DOUBLE_MULT: v = pow(eval(a->l),eval(a->r)); break;
  case 'M': v = -eval(a->r); break;

  /* comparisons */
  case GREAT: v = (eval(a->l) > eval(a->r))? 1 : 0; break;
  case LESS: v = (eval(a->l) < eval(a->r))? 1 : 0; break;
  case NOTEQ: v = (eval(a->l) != eval(a->r))? 1 : 0; break;
  case EQ: v = (eval(a->l) == eval(a->r))? 1 : 0; break;
  case GREATEQ: v = (eval(a->l) >= eval(a->r))? 1 : 0; break;
  case LESSEQ: v = (eval(a->l) <= eval(a->r))? 1 : 0; break;

  /* control flow */
  case 'I': 
    if( eval( ((struct flow *)a)->cond) != 0) {
      if( ((struct flow *)a)->tl) {
		v = eval( ((struct flow *)a)->tl);
      } 
    } else {
      if( ((struct flow *)a)->el) {
        v = eval(((struct flow *)a)->el);
      } 
    }
    break;

  /* stmt */
  case 'L': eval(a->l); v = eval(a->r); break;

  /*function call*/
  case 'C': v = calluser((struct ufncall *)a); break;

  /* print */
  case 'P': std::cout<<"result>> "<<eval(a->r)<<std::endl; break;

  /* return */
  case 'R': v = eval(a->r);	break;
  
  default: std::cout<<"internal error: bad node "<<(char)a->nodetype<<std::endl;
  }
  return v;
}

/*function call*/
double calluser(struct ufncall *f) {
	
  struct symbol *fn = f->s;	
  struct paramlist_formal *paramlist_form;		/* formal parameters */
  struct paramlist_actual *paramlist_act = f->l;	/* actual parameters */
  double *oldval, *newval;	/* saved arg values */
  double v;
  int nargs;
  int i;
  char s[100] = {0};
  
  if(!fn->stmt) {
	sprintf(s,"call to undefined function %s", fn->name);
    yyerror(s);
	free_mem();
	exit(0);
  }

  /* count the arguments */
  paramlist_form = fn->paramlist_form;
  for(nargs = 0; paramlist_form; paramlist_form = paramlist_form->next)
    nargs++;

  /* prepare to save them */
  oldval = (double *)malloc(nargs * sizeof(double));
  newval = (double *)malloc(nargs * sizeof(double));
  if(!oldval || !newval) {
    yyerror("Out of space ");
	exit(0);
  }
  
  /* evaluate the arguments */
  for(i = 0; i < nargs; i++) {
    if(!paramlist_act) {
	  sprintf(s, "too few args in call to %s", fn->name);
      yyerror(s);
      free(oldval); free(newval);
	  free_mem();
	  exit(0);
    }

     newval[i] = eval(paramlist_act->parm);
     paramlist_act = paramlist_act->next;
  }
		     
  /* save old values of dummies, assign new ones */
  paramlist_form = fn->paramlist_form;
  for(i = 0; i < nargs; i++) {
    struct symbol *s = paramlist_form->sym;

    oldval[i] = s->value;
    s->value = newval[i];
    paramlist_form = paramlist_form->next;
  }

  free(newval);

  /* evaluate the function */
  v = eval(fn->stmt);

  /* put the dummies back */
  paramlist_form = fn->paramlist_form;
  for(i = 0; i < nargs; i++) {
    struct symbol *s = paramlist_form->sym;

    s->value = oldval[i];
    paramlist_form = paramlist_form->next;
  }

  free(oldval);
  return v;
}

void ast_free(struct ast *a)
{
  if(!a) {
  	return;
    yyerror("internal error, null eval");
  }
  
  switch(a->nodetype) {
    /* constant */
  case 'K': free(a); break;

    /* name reference */
  case 'N': free(a); break;

    /* assignment */
  case '=': ast_free(((struct symasgn *)a)->v); free(a); break;

    /* expressions */
  case 'M': ast_free(a->r); free(a); break;	
  case PLUS:
  case MINUS:
  case MULT:
  case DIV:
  case MOD:
  case DOUBLE_MULT:

  /* comparisons */
  case LESS:
  case GREAT:
  case EQ:
  case NOTEQ:
  case GREATEQ:
  case LESSEQ: 
  	ast_free(a->l); ast_free(a->r); free(a); break;

  /* control flow */
  case 'I': 
  	ast_free( ((struct flow *)a)->cond);
	if( ((struct flow *)a)->tl)	ast_free( ((struct flow *)a)->tl);
	if( ((struct flow *)a)->el) ast_free(((struct flow *)a)->el);
	free(a);
    break;

  /*stmt*/
  case 'L': ast_free(a->l); ast_free(a->r); free(a); break;

  /*function call*/
  case 'C': free_paramlist_actual(((struct ufncall *)a)->l);
	  		free(a); break;

  /*print*/
  case 'P': 
  /*return*/	
  case 'R':
  			ast_free(a->r); free(a); break;
  
  default: std::cout<<"internal error: bad node "<<(char)a->nodetype<<std::endl;
  }
}

void evaluate(void){
	
	std::map<std::string, struct symbol>::iterator pos = symtab.find("main");
	if(pos!=symtab.end())	eval(pos->second.stmt);
	else std::cout<<"main function not found"<<std::endl;
}

void free_mem(void){
	std::map<std::string, struct symbol>::iterator pos;
	for(pos = symtab.begin(); pos!=symtab.end(); pos++){
		free(pos->second.name);
		ast_free(pos->second.stmt);
		free_paramlist_formal(pos->second.paramlist_form);
	}
}
