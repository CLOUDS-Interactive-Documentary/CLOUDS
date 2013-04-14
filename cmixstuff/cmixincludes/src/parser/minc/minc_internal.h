/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
/* written by John Gibson, based on the classic cmix Minc by Lars Graf */

#ifndef _MINC_INTERNAL_H_
#define _MINC_INTERNAL_H_ 1

#include <float.h>   /* for epsilon constants */
#include <maxdispargs.h>
#include "minc.h"

//#define DEBUG
#ifdef DEBUG
   #define DPRINT(msg)                    printf((msg))
   #define DPRINT1(msg, arg)              printf((msg), (arg))
   #define DPRINT2(msg, arg1, arg2)       printf((msg), (arg1), (arg2))
   #define DPRINT3(msg, arg1, arg2, arg3) printf((msg), (arg1), (arg2), (arg3))
   #define DPRINT4(msg, arg1, arg2, arg3, arg4) \
                                 printf((msg), (arg1), (arg2), (arg3), (arg4))
#else
   #define DPRINT(msg)
   #define DPRINT1(msg, arg)
   #define DPRINT2(msg, arg1, arg2)
   #define DPRINT3(msg, arg1, arg2, arg3)
   #define DPRINT4(msg, arg1, arg2, arg3, arg4)
#endif

/* important Minc tuning parameters */
#define YYLMAX   2048      /* maximum yacc line length */
#define MAXSTACK 15        /* depth of function call or list recursion */
#define HASHSIZE 107       /* number of buckets in string table */

typedef union {
   int ival;
   struct tree *trees;
   char *str;
} YYSTYPE;
#define YYSTYPE_IS_DECLARED   /* keep bison from declaring YYSTYPE as an int */

typedef enum {
   MincVoidType = 0,
   MincFloatType,       /* a floating point number, either float or double */
   MincStringType,
   MincHandleType,
   MincListType
} MincDataType;

//typedef float MincFloat;
//#define EPSILON FLT_EPSILON

typedef double MincFloat;
#define EPSILON DBL_EPSILON

typedef const char *MincString;
typedef void *MincHandle;  // contents of this is opaque to Minc

/* A MincList contains an array of MincListElem's, whose underlying data
   type is flexible.  So a MincList is an array of arbitrarily mixed types
   (any of the types represented in the MincDataType enum), and it can
   support nested lists.
*/

typedef struct {
   int len;                /* number of MincListElem's in <data> array */
   int refcount;			/* reference count for contained data */
   struct _minc_list_elem *data;
} MincList;

typedef union {
   MincFloat number;
   MincString string;
   MincHandle handle;
   MincList *list;
} MincValue;

typedef struct _minc_list_elem {
   MincDataType type;
   MincValue val;
} MincListElem;


/* scopes */
typedef enum {
   S_LOCAL = 3,
   S_PARAM = 4,
   S_GLOBAL = 5,
   S_RESERVED = 10       /* the smallest number the newscope returns */
} ScopeType;

typedef struct symbol {       /* symbol table entries */
   struct symbol *next;       /* next entry on hash chain */
   ScopeType scope;
   MincDataType type;         /* type of data represented by symbol */
   char *name;                /* symbol name */
   MincValue v;
#ifdef NOTYET
   short defined;             /* set when function defined */
   short offset;              /* offset in activation frame */
   Symbol *plist;             /* next parameter in parameter list */
#endif
} Symbol;


/* intermediate tree representation */

typedef enum {
   NodeZero = 0,
   NodeSeq,
   NodeStore,
   NodeList,
   NodeListElem,
   NodeEmptyListElem,
   NodeSubscriptRead,
   NodeSubscriptWrite,
   NodeOpAssign,
   NodeName,
   NodeConstf,
   NodeString,
   NodeCall,
   NodeAnd,
   NodeOr,
   NodeOperator,
   NodeUnaryOperator,
   NodeNot,
   NodeRelation,
   NodeIf,
   NodeWhile,
   NodeFor,
   NodeIfElse,
   NodeNoop
} NodeKind;

typedef enum {
   OpZero = 0,
   OpFree,
   OpPlus,
   OpMinus,
   OpMul,
   OpDiv,
   OpMod,
   OpPow,
   OpNeg,
   OpEqual,
   OpNotEqual,
   OpLess,
   OpGreater,
   OpLessEqual,
   OpGreaterEqual
} OpKind;

typedef struct tree {
   NodeKind kind;
   MincDataType type;
   OpKind op;
   union {
      struct tree *child[4];
      Symbol *symbol;
      double number;
      char *string;
   } u;
   MincValue v;
   char *funcname;              /* for NodeCall only */
} *Tree;


/* prototypes for internal Minc use */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* builtin.c */
int call_builtin_function(const char *funcname, const MincListElem arglist[],
   const int nargs, MincListElem *retval);

/* callextfunc.c */
int call_external_function(const char *funcname, const MincListElem arglist[],
   const int nargs, MincListElem *return_value);
MincHandle minc_binop_handle_float(const MincHandle handle, const MincFloat val, OpKind op);
MincHandle minc_binop_float_handle(const MincFloat val, const MincHandle handle, OpKind op);
MincHandle minc_binop_handles(const MincHandle handle1, const MincHandle handle2, OpKind op);

/* error.c */
void sys_error(char *msg);
void minc_advise(const char *msg, ...);
void minc_warn(const char *msg, ...);
void minc_die(const char *msg, ...);
void minc_internal_error(const char *msg, ...);
#ifdef MAXMSP
void mm_yyerror(char *msg);
#else
void yyerror(char *msg);
#endif

/* sym.c */
struct symbol *install(char *name, ScopeType scope);
struct symbol *lookup(char *name);
char *strsave(char *str);
char *emalloc(long nbytes);
void efree(void *mem);
void clear_elem(MincListElem *);
void unref_value_list(MincValue *);
void free_symbols();

/* trees.c */
Tree tnoop(void);
Tree tseq(Tree e1, Tree e2);
Tree top(OpKind op, Tree e1, Tree e2);
Tree tunop(OpKind op, Tree e1);
Tree tbuiltinfunc(OpKind op, Tree e1);
Tree tstore(Tree e1, Tree e2);
Tree tlist(Tree e1);
Tree tlistelem(Tree e1, Tree args);
Tree temptylistelem(void);
Tree tsubscriptread(Tree e1, Tree e2);
Tree tsubscriptwrite(Tree e1, Tree e2, Tree e3);
Tree topassign(Tree e1, Tree e2, OpKind op);
Tree tname(Symbol *symbol);
Tree tstring(char *str);
Tree tconstf(MincFloat num);
Tree tcall(Tree args, char *funcname);
Tree tcand(Tree test1, Tree test2);
Tree tcor(Tree test1, Tree test2);
Tree tnot(Tree test1);
Tree trel(OpKind op, Tree e1, Tree e2);
Tree tif(Tree e1, Tree e2);
Tree tifelse(Tree e1, Tree e2, Tree e3);
Tree tfor(Tree e1, Tree e2, Tree e3, Tree e4);
Tree twhile(Tree e1, Tree e2);
Tree exct(Tree tp);
void free_tree(Tree tp);

/* utils.c */
int is_float_list(const MincList *list);
MincFloat *float_list_to_array(const MincList *list);
MincList *array_to_float_list(const MincFloat *array, const int len);

/* y.tab.c */
void declare(MincDataType type);
Tree go(Tree t1);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _MINC_INTERNAL_H_ */
