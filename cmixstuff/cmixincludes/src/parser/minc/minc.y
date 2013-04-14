/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
%{
#include <string.h>
#include "minc_internal.h"
#include "lex.yy.c"
#define YYDEBUG 1
#define MAXTOK_IDENTLIST 200

#undef MDEBUG	/* turns on some parser debugging below */

#ifdef MDEBUG
#define MPRINT(x) printf(x "\n")
#define MPRINT1(x,y) printf(x "\n", y)
#else
#define MPRINT(x)
#define MPRINT1(x,y)
#endif

static Tree		program;
static Symbol	*sym;
static int		idcount = 0;	
static char		*idlist[MAXTOK_IDENTLIST];  
static int		flerror;		/* set if there was an error during parsing */
static int		level = 0;	/* keeps track whether we are in a structure */
static void cleanup();
%}

%left  <ival> LOWPRIO
%left  <ival> '='
%left  <ival> TOK_MINUSEQU
%left  <ival> TOK_PLUSEQU
%left  <ival> TOK_DIVEQU
%left  <ival> TOK_MULEQU
%left  <ival> TOK_OR
%left  <ival> TOK_AND
%left  <ival> TOK_EQU TOK_UNEQU
%left  <ival> '<' '>' TOK_LESSEQU TOK_GTREQU
%left  <ival> '+' '-'
%left  <ival> '*' '/'
%left  <ival> TOK_POW
%left  <ival> CASTTOKEN
%token <ival> TOK_FLOAT_DECL
%token <ival> TOK_STRING_DECL
%token <ival> TOK_HANDLE_DECL
%token <ival> TOK_IDENT TOK_NUM TOK_NOT TOK_IF TOK_ELSE TOK_FOR TOK_WHILE 
%token <ival> TOK_TRUE TOK_FALSE TOK_STRING 
%type  <trees> stml stmt rstmt bexp expl exp str
%type  <str> id 

%%
/* program (the "start symbol") */
prg:	| stml			{ MPRINT("prg:"); program = $1; cleanup(); return 0; }
	;
 
/* statement list */
stml:	stmt				{ MPRINT("<stmt>"); $$ = $1; }
	| stmt ';'			{ MPRINT("<stmt;>"); $$ = $1; }
	| stml stmt			{ MPRINT("<stml stmt>"); $$ = tseq($1, $2); }
	| stml stmt ';'	{ MPRINT("<stml stmt;>"); $$ = tseq($1, $2); }
	;

/* statement */
stmt: rstmt				{ MPRINT("<rstmt>");
								if (level == 0) 
									$$ = go($1); 
								else
									$$ = $1;
							}
	| TOK_FLOAT_DECL idl	{ declare(MincFloatType); idcount = 0; }
	| TOK_STRING_DECL idl	{ declare(MincStringType); idcount = 0; }
	| TOK_HANDLE_DECL idl	{ declare(MincHandleType); idcount = 0; }
	| TOK_IF level bexp stmt {
								level--; MPRINT1("level %d", level);
								$$ = go(tif($3, $4));
							}
	| TOK_IF level bexp stmt TOK_ELSE stmt {
								level--; MPRINT1("level %d", level);
								$$ = go(tifelse($3, $4, $6));
							}
	| TOK_WHILE level bexp stmt	{
								level--; MPRINT1("level %d", level);
								$$ = go(twhile($3, $4));
							}
	| TOK_FOR level '(' stmt ';' bexp ';' stmt ')' stmt {
								level--; MPRINT1("level %d", level);
								$$ = go(tfor($4, $6, $8, $10));
							}
	| '{' stml '}'		{ $$ = $2; }
	| error TOK_FLOAT_DECL	{ flerror = 1; $$ = tnoop(); }
	| error TOK_STRING_DECL	{ flerror = 1; $$ = tnoop(); }
	| error TOK_HANDLE_DECL	{ flerror = 1; $$ = tnoop(); }
	| error TOK_IF		{ flerror = 1; $$ = tnoop(); }
	| error TOK_WHILE	{ flerror = 1; $$ = tnoop(); }
	| error TOK_FOR	{ flerror = 1; $$ = tnoop(); }
	| error '{'			{ flerror = 1; $$ = tnoop(); }
	| error TOK_ELSE	{ flerror = 1; $$ = tnoop(); }
	| error ';'			{ flerror = 1; $$ = tnoop(); }
	;

/* statement nesting level counter */
level:  /* nothing */ { level++; MPRINT1("level %d", level); }
	;

/* statement returning a value: assignments, function calls, etc. */
rstmt: id '=' exp		{
								sym = lookup($1);
								if (sym == NULL)	/* then autodeclare it */
									sym = install($1, S_GLOBAL);
								$$ = tstore(tname(sym), $3);
							}
	| id TOK_PLUSEQU exp {
								sym = lookup($1);
								if (sym == NULL) {
									minc_die("'%s' is not declared", $1);
									$$ = tnoop();
								}
								else
									$$ = topassign(tname(sym), $3, OpPlus);
							}
	| id TOK_MINUSEQU exp {
								sym = lookup($1);
								if (sym == NULL) {
									minc_die("'%s' is not declared", $1);
									$$ = tnoop();
								}
								else
									$$ = topassign(tname(sym), $3, OpMinus);
							}
	| id TOK_MULEQU exp {
								sym = lookup($1);
								if (sym == NULL) {
									minc_die("'%s' is not declared", $1);
									$$ = tnoop();
								}
								else
									$$ = topassign(tname(sym), $3, OpMul);
							}
	| id TOK_DIVEQU exp {
								sym = lookup($1);
								if (sym == NULL) {
									minc_die("'%s' is not declared", $1);
									$$ = tnoop();
								}
								else
									$$ = topassign(tname(sym), $3, OpDiv);
							}

	| id '(' expl ')' { $$ = tcall($3, $1); }

/* XXX: This causes 1 reduce/reduce conflict on '}'  How bad is this?  -JGG */
	| '{' expl '}' 	{ $$ = tlist($2); }

	| id '[' exp ']' 	{
								sym = lookup($1);
								if (sym == NULL) {
									minc_die("'%s' is not declared", $1);
									$$ = tnoop();
								}
								else
									$$ = tsubscriptread(tname(sym), $3);
							}
	| id '[' exp ']' '=' exp {
								sym = lookup($1);
								if (sym == NULL) {
									minc_die("'%s' is not declared", $1);
									$$ = tnoop();
								}
								else
									$$ = tsubscriptwrite(tname(sym), $3, $6);
							}
	;

/* identifier list */
idl: id					{ idlist[idcount++] = $1; }
	| idl ',' id		{ idlist[idcount++] = $3; }
	;

/* identifier */
id:  TOK_IDENT			{ $$ = strsave(yytext); }
	;

/* expression list */
expl:	exp				{ $$ = tlistelem(temptylistelem(), $1); }
	| expl ',' exp		{ $$ = tlistelem($1, $3); }
/* XXX causes reduce/reduce conflicts; don't need because str -> exp below
	| str	 				{ $$ = tlistelem(temptylistelem(), $1); }
	| expl ',' str		{ $$ = tlistelem($1, $3); }
*/
	| /* nothing */	{ $$ = temptylistelem(); }
	;

/* string */
str:	TOK_STRING		{
								char *s = yytext + 1;
								s[strlen(s) - 1] = '\0';
								$$ = tstring(strsave(s));
							}
	;

/* Boolean expression */
bexp:	exp %prec LOWPRIO	{ $$ = $1; }
	| TOK_NOT bexp %prec TOK_UNEQU { $$ = tnot($2); }
	| bexp TOK_AND bexp	{ $$ = tcand($1, $3); }
	| bexp TOK_OR  bexp	{ $$ = tcor($1, $3); }
	| bexp TOK_EQU bexp	{ $$ = trel(OpEqual, $1, $3); }
	| exp TOK_UNEQU exp	{ $$ = trel(OpNotEqual, $1, $3); }
	| exp '<' exp			{ $$ = trel(OpLess, $1, $3); }
	| exp '>' exp			{ $$ = trel(OpGreater, $1, $3); }
	| exp TOK_LESSEQU exp { $$ = trel(OpLessEqual, $1, $3); }
	| exp TOK_GTREQU exp	{ $$ = trel(OpGreaterEqual, $1, $3); }
	| TOK_TRUE				{ $$ = trel(OpEqual, tconstf(1.0), tconstf(1.0)); }
	| TOK_FALSE				{ $$ = trel(OpNotEqual, tconstf(1.0), tconstf(1.0)); }
	;

/* expression */
exp: rstmt				{ $$ = $1; }
	| exp TOK_POW exp	{ $$ = top(OpPow, $1, $3); }
	| exp '*' exp		{ $$ = top(OpMul, $1, $3); }
	| exp '/' exp		{ $$ = top(OpDiv, $1, $3); }
	| exp '+' exp		{ $$ = top(OpPlus, $1, $3); }
	| exp '-' exp		{ $$ = top(OpMinus, $1, $3); }
	| exp '%' exp		{ $$ = top(OpMod, $1, $3); }
	| '(' bexp ')'		{ $$ = $2; }
	| str					{ $$ = $1; }
	| TOK_NUM			{
								double f = atof(yytext);
								$$ = tconstf(f);
							}
	| TOK_TRUE			{ $$ = tconstf(1.0); }
	| TOK_FALSE			{ $$ = tconstf(0.0); }
	| '-' exp %prec CASTTOKEN {
								/* tconstf is a dummy; makes exct_operator work */
								$$ = top(OpNeg, $2, tconstf(0.0));
							}
	| id					{
								sym = lookup($1);
								if (sym == NULL) {
// FIXME: install id w/ value of 0, then warn??
									minc_die("'%s' is not declared", $1);
									$$ = tconstf(0.0);
								}
								else
									$$ = tname(sym);
							}
	;


%%

void
declare(MincDataType type)
{
	int i;

	for (i = 0; i < idcount; i++) {
	   Symbol *sym = lookup(idlist[i]);
		if (sym != NULL) {
			minc_warn("variable redefined: %s", idlist[i]);
			/* note this handling may be illegal in arbitrary scoping */
			sym->type = type;
		}
		else {
			sym = install(idlist[i], S_GLOBAL);
			sym->type = type;
		}
	}
}

#define FREE_TREES_AT_END

Tree
go(Tree t1)
{
	MPRINT("go()");
	if (level == 0) {
		exct(t1);
#ifndef FREE_TREES_AT_END
		free_tree(t1);
#endif
	}
	return t1;
}

int yywrap()
{
	return 1;
}

static void cleanup()
{
#ifdef FREE_TREES_AT_END
    free_tree(program);
#else
	efree(program);
#endif
	free_symbols();
#ifdef MAXMSP
	yy_delete_buffer(yy_current_buffer);
	yy_current_buffer = NULL;
#endif
	yy_delete_buffer(YY_CURRENT_BUFFER);
	/* yy_current_buffer = NULL; */
}

