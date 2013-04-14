/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/* symbol table management routines */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minc_internal.h"
#include "handle.h"

static struct symbol *htab[HASHSIZE] =
   {0};                         /* hash table */

static struct str {             /* string table */
   char *str;                   /* string */
   struct str *next;            /* next entry */
} *stab[HASHSIZE] = {
   0
};

static struct symbol *freelist = NULL;  /* free list of unused entries */

/* prototypes for local functions */
static struct symbol *symalloc(char *name);
#ifdef NOTYET
static void free_node(struct symbol *p);
static void kill_scope(ScopeType scope);
#endif
static char *dname(int x);
static void dump(struct symbol *p, FILE * fp);
static int hash(char *s);


/* Allocate and initialize and new symbol table entry for <name>. */
static struct symbol *
symalloc(char *name)
{
   struct symbol *p;

   p = freelist;
   if (p)
      freelist = p->next;
   else {
      p = (struct symbol *) emalloc(sizeof(struct symbol));
      if (p == NULL)
         return NULL;
   }
   p->name = name;
#ifdef NOTYET
   p->defined = p->offset = 0;
   p->list = NULL;
#endif
   return p;
}

void
free_symbols()
{
	int s;
#ifdef DEBUG
	printf("freeing symbol and string tables...\n");
#endif
	for (s = 0; s < HASHSIZE; ++s)
	{
		struct symbol *p;
		struct str *str;
   		for (p = htab[s]; p != NULL; ) {
			struct symbol *next = p->next;
#ifdef DEBUG
			printf("\tfreeing symbol \"%s\" (%p)\n", p->name, p);
#endif
			if (p->type == MincHandleType)
				unref_handle(p->v.handle);
			else if (p->type == MincListType) {
				unref_value_list(&p->v);
			}
			free(p);
			p = next;
		}
		htab[s] = NULL;
#if 1
		for (str = stab[s]; str != NULL; ) {
			struct str *next = str->next;
			free(str->str);
			free(str);
			str = next;
		}
#endif
	}
#ifdef DEBUG
	printf("done\n");
#endif
}

#ifdef NOTYET
/* Free storage for reuse.  Very closely connected to symalloc.
   TBD:  only allow a maximum freelist length
*/
static void
free_node(struct symbol *p)
{
   if (p == NULL) {
      minc_warn("free_node was called with NULL ptr ");
      return;
   }

   if (freelist == NULL)
      freelist = p;
   else {
      p->next = freelist;
      freelist = p;
   }
}
#endif


/* Allocate a new entry for name and install it. */
struct symbol *
install(char *name, ScopeType scope)
{
   int h;
   struct symbol *p;

   p = symalloc(name);
   h = hash(name);
   p->next = htab[h];
   p->scope = scope;
   p->type = MincVoidType;
   htab[h] = p;
   p->v.number = 0.0;

   DPRINT2("install ('%s') => %p\n", name, p);
   return p;
}


/* Lookup <name>; return pointer to entry. */
/* WARNING: it can only find symbol if name is a ptr returned by strsave */
struct symbol *
lookup(char *name)
{
   struct symbol *p = NULL;

   for (p = htab[hash(name)]; p != NULL; p = p->next)
      if (name == p->name)
         break;

   DPRINT2("lookup ('%s') => %p\n", name, p);
   return p;
}


/* Lookup <str> and install if necessary; return pointer. */
char *
strsave(char *str)
{
   int h;
   struct str *p;

   h = hash(str);
   for (p = stab[h]; p != NULL; p = p->next)
      if (strcmp(str, p->str) == 0)
         return (p->str);
   p = (struct str *) emalloc(sizeof(struct str));
   if (p == NULL)
      return NULL;
   p->str = (char *) emalloc(strlen(str) + 1);
   if (p->str == NULL)
      return NULL;
   strcpy(p->str, str);
   p->next = stab[h];
   stab[h] = p;

   DPRINT2("strsave ('%s') => %p\n", str, p);
   return p->str;
}


#ifdef NOTYET
/* remove all entries of the scope from the symbol table */
static void
kill_scope(ScopeType scope)
{
   register int i;
   register struct symbol *p, **q, *r;

   for (i = 0; i < HASHSIZE; i++) {
      q = &htab[i];
      for (p = htab[i]; p; p = r) {
         r = p->next;
         if (p->scope == scope) {
            *q = p->next;
            free_node(p);
         }
         else
            q = &p->next;
      }
   }
}
#endif


#ifdef NOMORE
/* Return string representation of type or scope. */
static char *
dname(int x)
{
   static struct tname {
      int val;
      char *name;
   } tnames[] = {
      { T_INT, "int" },
      { T_FLOAT, "float" },
      { T_COND, "conditional" },
      { T_SCALAR, "scalar" },
      { T_FUNC, "()" },
      { T_ARRAY, "[]" },
      { S_GLOBAL, "global" },
      { S_PARAM, "parameter" },
      { S_LOCAL, "local" },
      { 0, 0 }
   };
   static char buf[30];
   int i;

   for (i = 0; tnames[i].name; i++)
      if (tnames[i].val == x)
         return (tnames[i].name);
   sprintf(buf, "<%d>", x);
   return buf;
}


/* Print entire symbol table or one entry. */
static void
dump(struct symbol *p, FILE * fp)
{
   int i;

   if (fp == NULL)
      fp = stderr;
   if (p == NULL)
      for (i = 0; i < HASHSIZE; i++)
         for (p = htab[i]; p; p = p->next)
            dump(p, fp);
   else {
      fprintf(fp, "%s ", dname(p->type));
/*
   fprintf(fp, "%s%s, %sscope=%s, offset=%d\n", p->name,
   dname(xshape(p->type)), p->defined?"defined, ":"",
   dname(p->scope), p->offset);
*/
   }
}
#endif /* NOMORE */


/* Has error-checking for malloc built in. */
char *
emalloc(long nbytes)
{
   char *s;

   s = (char *) malloc(nbytes);
   if (s == NULL)
      sys_error("system out of memory");

#ifndef NO_EMALLOC_DEBUG
   DPRINT2("emalloc: nbytes=%d, ptr=%p\n", nbytes, s);
#endif
   return s;
}

void efree(void *mem)
{
#ifndef NO_EMALLOC_DEBUG
   DPRINT1("efree: ptr=%p\n", mem);
#endif
   free(mem);
}

/* Returns an index to a hash bucket. */
static int
hash(char *s)
{
   int i = 0;

   while (*s) {
      i = (((unsigned int) *s + i) % HASHSIZE);
      s++;
   }
   return i;
}


