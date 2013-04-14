/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/* A revised MinC, supporting lists, types and other fun things.
   Based heavily on the classic cmix version by Lars Graf.
   Doug Scott added the '#' and '//' comment parsing.

   John Gibson <johgibso at indiana dot edu>, 1/20/04
*/

/* This file holds the intermediate tree representation. */

#include "minc_internal.h"
#include "handle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>


/* We maintain a stack of MAXSTACK lists, which we access when forming 
   user lists (i.e., {1, 2, "foo"}) and function argument lists.  Each
   element of this stack is a list, allocated and freed by push_list and
   pop_list.  <list> is an array of MincListElem structures, each having
   a type and a value, which is encoded in a MincValue union.  Nested lists
   and lists of mixed types are possible.
*/
static MincListElem *list;
static int list_len;
static MincListElem *list_stack[MAXSTACK];
static int list_len_stack[MAXSTACK];
static int list_stack_ptr;

#ifdef DEBUG
static int numTrees = 0;
static char *s_NodeKinds[] = {
   "NodeZero",
   "NodeSeq",
   "NodeStore",
   "NodeList",
   "NodeListElem",
   "NodeEmptyListElem",
   "NodeSubscriptRead",
   "NodeSubscriptWrite",
   "NodeOpAssign",
   "NodeName",
   "NodeConstf",
   "NodeString",
   "NodeCall",
   "NodeAnd",
   "NodeOr",
   "NodeOperator",
   "NodeUnaryOperator",
   "NodeNot",
   "NodeRelation",
   "NodeIf",
   "NodeWhile",
   "NodeFor",
   "NodeIfElse",
   "NodeNoop"
};

static const char *printNodeKind(NodeKind k)
{
	return s_NodeKinds[k];
}

#endif

/* prototypes for local functions */
static int cmp(MincFloat f1, MincFloat f2);
static Tree node(OpKind op, NodeKind kind);
static void push_list(void);
static void pop_list(void);

static void copy_tree_tree(Tree tpdest, Tree tpsrc);
static void copy_sym_tree(Tree tpdest, Symbol *src);
static void copy_tree_sym(Symbol *dest, Tree tpsrc);
static void copy_tree_listelem(MincListElem *edest, Tree tpsrc);
static void copy_listelem_tree(Tree tpdest, MincListElem *esrc);
static void copy_listelem_elem(MincListElem *edest, MincListElem *esrc);

/* floating point comparisons:
     f1 < f2   ==> -1
     f1 == f2  ==> 0
     f1 > f2   ==> 1 
*/
static int
cmp(MincFloat f1, MincFloat f2)
{
   if (fabs((double) f1 - (double) f2) < EPSILON) {
      /* printf("cmp=%g %g %g \n",f1,f2,fabs(f1-f2)); */
      return 0;
   }
   if ((f1 - f2) > EPSILON) { 
      /* printf("cmp > %g %g %g \n",f1,f2,fabs(f1-f2)); */
      return 1;
   }
   if ((f2 - f1) > EPSILON) {
      /* printf("cmp <%g %g %g \n",f1,f2,fabs(f1-f2)); */
      return -1;
   }
   return 0;
}

static MincList *
newList(int len)
{
   MincList *list = (MincList *) emalloc(sizeof(MincList));
   if (list) {
	  list->len = len;
	  list->refcount = 0;
	  if (len > 0) {
         list->data = (MincListElem *) emalloc(len * sizeof(MincListElem));
		 if (!list->data) {
			efree(list);
			list = NULL;
		 }
		 else {
            memset(list->data, 0, len * sizeof(MincListElem));
		 }
	  }
	  else
         list->data = NULL;
      DPRINT2("newList: %p alloc'd at len %d\n", list, list_len);
   }
   return list;
}

static void
resizeList(MincList *list, int newLen)
{
   int i;
   list->data = (MincListElem *) realloc(list->data, sizeof(MincListElem) * newLen);
   for (i = list->len; i < newLen; i++) {
	  list->data[i].type = MincFloatType;
	  list->data[i].val.number = 0.0;
   }
   list->len = newLen;
}

/* ========================================================================== */
/* Tree nodes */

static Tree
node(OpKind op, NodeKind kind)
{
   Tree tp;

   tp = (Tree) emalloc(sizeof(struct tree));
   if (tp == NULL)
      return NULL;
   tp->op = op;
   tp->kind = kind;
   tp->type = MincVoidType;
   tp->u.child[0] = NULL;    /* these clear entire <u> union */
   tp->u.child[1] = NULL;
   tp->u.child[2] = NULL;
   tp->u.child[3] = NULL;
   tp->v.list = NULL;
   tp->funcname = NULL;
#ifdef DEBUG
	++numTrees;
   DPRINT1("[%d trees in existence]\n", numTrees);
#endif
   return tp;
}


Tree
tnoop()
{
   Tree tp = node(OpFree, NodeNoop);

   DPRINT1("tnoop => %p\n", tp);
   return tp;
}


Tree
tseq(Tree e1, Tree e2)
{
   Tree tp = node(OpFree, NodeSeq);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;
   tp->u.child[1] = e2;

   DPRINT3("tseq (%p, %p) => %p\n", e1, e2, tp);
   return tp;
}


Tree
top(OpKind op, Tree e1, Tree e2)
{
   Tree tp = node(op, NodeOperator);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;
   tp->u.child[1] = e2;

   DPRINT4("top (%d, %p, %p) => %p\n", op, e1, e2, tp);
   return tp;
}


Tree
tunop(OpKind op, Tree e1)
{
   Tree tp = node(op, NodeUnaryOperator);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;

   DPRINT3("tunop (%d, %p) => %p\n", op, e1, tp);
   return tp;
}


/* store a value into a variable */
Tree
tstore(Tree e1, Tree e2)
{
   Tree tp = node(OpFree, NodeStore);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;
   tp->u.child[1] = e2;

   DPRINT3("tstore (%p, %p) => %p\n", e1, e2, tp);
   return tp;
}


/* like tstore, but modify value before storing into variable */
Tree
topassign(Tree e1, Tree e2, OpKind op)
{
   Tree tp = node(op, NodeOpAssign);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;
   tp->u.child[1] = e2;

   DPRINT4("topassign, op=%d (%p, %p) => %p\n", op, e1, e2, tp);
   return tp;
}


/* converts symbol table entry into tree
   or initialize tree node to a symbol entry
*/
Tree
tname(Symbol *symbol)
{
   Tree tp = node(OpFree, NodeName);
   if (tp == NULL)
      return NULL;

   tp->u.symbol = symbol;

   DPRINT2("tname ('%s') => %p\n", symbol->name, tp);
   return tp;
}


Tree
tstring(char *str)
{
   Tree tp = node(OpFree, NodeString);
   if (tp == NULL)
      return NULL;

   tp->u.string = str;

   DPRINT2("tstring ('%s') => %p\n", str, tp);
   return tp;
}


Tree
tconstf(MincFloat num)
{
   Tree tp = node(OpFree, NodeConstf);
   if (tp == NULL)
      return NULL;

   tp->u.number = num;

   DPRINT2("tconstf (%f) => %p\n", num, tp);
   return tp;
}


Tree
tcall(Tree args, char *funcname)
{
   Tree tp = node(OpFree, NodeCall);
   if (tp == NULL)
      return NULL;

   tp->funcname = funcname;
   tp->u.child[0] = args;

   DPRINT3("tcall ('%s', %p) => %p\n", funcname, args, tp);
   return tp;
}


Tree
tcand(Tree test1, Tree test2)
{
   Tree tp = node(OpFree, NodeAnd);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = test1;
   tp->u.child[1] = test2;

   DPRINT3("tcand (%p, %p) => %p\n", test1, test2, tp);
   return tp;
}


Tree
tcor(Tree test1, Tree test2)
{
   Tree tp = node(OpFree, NodeOr);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = test1;
   tp->u.child[1] = test2;

   DPRINT3("tcor (%p, %p) => %p\n", test1, test2, tp);
   return tp;
}


Tree
tnot(Tree test1)
{
   Tree tp = node(OpFree, NodeNot);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = test1;

   DPRINT2("tnot (%p) => %p\n", test1, tp);
   return tp;
}


Tree
trel(OpKind op, Tree e1, Tree e2)
{
   Tree tp = node(op, NodeRelation);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;
   tp->u.child[1] = e2;

   DPRINT4("trel (%d, %p, %p) => %p\n", op, e1, e2, tp);
   return tp;
}


/* Create list: either an argument list or a user array.  Why do we
   not separate these two things?  Because at the time when we need
   to push the list elements onto a stack, we don't know whether they
   form part of a user list or an argument list.
*/
Tree
tlist(Tree e1)
{
   Tree tp = node(OpFree, NodeList);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;

   DPRINT2("tlist (%p) => %p\n", e1, tp);
   return tp;
}


Tree
tlistelem(Tree e1, Tree e2)
{
   Tree tp = node(OpFree, NodeListElem);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;
   tp->u.child[1] = e2;

   DPRINT3("tlistelem (%p, %p) => %p\n", e1, e2, tp);
   return tp;
}


Tree
temptylistelem()
{
   Tree tp = node(OpFree, NodeEmptyListElem);

   DPRINT1("temptylistelem => %p\n", tp);
   return tp;
}


Tree
tsubscriptread(Tree e1, Tree e2)
{
   Tree tp = node(OpFree, NodeSubscriptRead);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;
   tp->u.child[1] = e2;

   DPRINT3("tsubscriptread (%p, %p) => %p\n", e1, e2, tp);
   return tp;
}


Tree
tsubscriptwrite(Tree e1, Tree e2, Tree e3)
{
   Tree tp = node(OpFree, NodeSubscriptWrite);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;
   tp->u.child[1] = e2;
   tp->u.child[2] = e3;

   DPRINT4("tsubscriptwrite (%p, %p, %p) => %p\n", e1, e2, e3, tp);
   return tp;
}


Tree
tif(Tree e1, Tree e2)
{
   Tree tp = node(OpFree, NodeIf);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;
   tp->u.child[1] = e2;

   DPRINT3("tif (%p, %p) => %p\n", e1, e2, tp);
   return tp;
}


Tree
tifelse(Tree e1, Tree e2, Tree e3)
{
   Tree tp = node(OpFree, NodeIfElse);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;
   tp->u.child[1] = e2;
   tp->u.child[2] = e3;

   DPRINT4("tifelse (%p, %p, %p) => %p\n", e1, e2, e3, tp);
   return tp;
}


Tree
tfor(Tree e1, Tree e2, Tree e3, Tree e4)
{
   Tree tp = node(OpFree, NodeFor);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;
   tp->u.child[1] = e2;
   tp->u.child[2] = e3;
   tp->u.child[3] = e4;

   DPRINT4("tfor (%p, %p, %p, <e4>) => %p\n", e1, e2, e3, tp);
   return tp;
}


Tree
twhile(Tree e1, Tree e2)
{
   Tree tp = node(OpFree, NodeWhile);
   if (tp == NULL)
      return NULL;

   tp->u.child[0] = e1;
   tp->u.child[1] = e2;

   DPRINT3("twhile (%p, %p) => %p\n", e1, e2, tp);
   return tp;
}


/* ========================================================================== */
/* Operators */

/* ---------------------------------------------------------- do_op_string -- */
static void
do_op_string(Tree tp, const char *str1, const char *str2, OpKind op)
{
   char *s;
   int   len;

   switch (op) {
      case OpPlus:   /* concatenate */
         len = (strlen(str1) + strlen(str2)) + 1;
         s = (char *) emalloc(sizeof(char) * len);
         if (s == NULL)
            return;
         strcpy(s, str1);
         strcat(s, str2);
         tp->v.string = s;
         // printf("str1=%s, str2=%s len=%d, s=%s\n", str1, str2, len, s);
         break;
      case OpMinus:
      case OpMul:
      case OpDiv:
      case OpMod:
      case OpPow:
      case OpNeg:
         minc_warn("unsupported operation on a string");
         return;
      default:
         minc_internal_error("invalid string operator");
         break;
   }
   tp->type = MincStringType;
}


/* ------------------------------------------------------------- do_op_num -- */
static void
do_op_num(Tree tp, const MincFloat val1, const MincFloat val2, OpKind op)
{
   switch (op) {
      case OpPlus:
         tp->v.number = val1 + val2;
         break;
      case OpMinus:
         tp->v.number = val1 - val2;
         break;
      case OpMul:
         tp->v.number = val1 * val2;
         break;
      case OpDiv:
         tp->v.number = val1 / val2;
         break;
      case OpMod:
         tp->v.number = (MincFloat) ((long) val1 % (long) val2);
         break;
      case OpPow:
         tp->v.number = pow(val1, val2);
         break;
      case OpNeg:
         tp->v.number = -val1;        /* <val2> ignored */
         break;
      default:
         minc_internal_error("invalid numeric operator");
         break;
   }
   tp->type = MincFloatType;
}


/* ------------------------------------------------------ do_op_handle_num -- */
static void
do_op_handle_num(Tree tp, const MincHandle val1, const MincFloat val2,
      OpKind op)
{
   switch (op) {
      case OpPlus:
      case OpMinus:
      case OpMul:
      case OpDiv:
      case OpMod:
      case OpPow:
         tp->v.handle = minc_binop_handle_float(val1, val2, op);
         ref_handle(tp->v.handle);
         break;
      case OpNeg:
         tp->v.handle = minc_binop_handle_float(val1, -1.0, OpMul);	// <val2> ignored
         ref_handle(tp->v.handle);
         break;
      default:
         minc_internal_error("invalid operator for handle and number");
         break;
   }
   tp->type = MincHandleType;
}


/* ------------------------------------------------------ do_op_num_handle -- */
static void
do_op_num_handle(Tree tp, const MincFloat val1, const MincHandle val2,
      OpKind op)
{
   switch (op) {
      case OpPlus:
      case OpMinus:
      case OpMul:
      case OpDiv:
      case OpMod:
      case OpPow:
         tp->v.handle = minc_binop_float_handle(val1, val2, op);
         ref_handle(tp->v.handle);
         break;
      case OpNeg:
         /* fall through */
      default:
         minc_internal_error("invalid operator for handle and number");
         break;
   }
   tp->type = MincHandleType;
}


/* --------------------------------------------------- do_op_handle_handle -- */
static void
do_op_handle_handle(Tree tp, const MincHandle val1, const MincHandle val2,
      OpKind op)
{
	switch (op) {
	case OpPlus:
	case OpMinus:
	case OpMul:
	case OpDiv:
	case OpMod:
	case OpPow:
		tp->v.handle = minc_binop_handles(val1, val2, op);
        ref_handle(tp->v.handle);
		break;
	case OpNeg:
	default:
		minc_internal_error("invalid binary handle operator");
		break;
	}
	if (tp->v.handle)
		tp->type = MincHandleType;
}


/* ---------------------------------------------------- do_op_list_iterate -- */
/* Iterate over <child>'s list, performing the operation specified by <op>,
   using the scalar <val>, for each list element.  Store the result into a
   new list for <tp>, so that child's list is unchanged.
*/
static void
do_op_list_iterate(Tree tp, Tree child, const MincFloat val, const OpKind op)
{
   int i;
   MincListElem *dest;
   const MincList *srcList = child->v.list;
   const int len = srcList->len;
   MincListElem *src = srcList->data;
   MincList *destList = newList(len);
   if (destList == NULL)
      return;
   dest = destList->data;
   assert(len >= 0);
   switch (op) {
      case OpPlus:
         for (i = 0; i < len; i++) {
            if (src[i].type == MincFloatType)
               dest[i].val.number = src[i].val.number + val;
            else
               dest[i].val = src[i].val;
            dest[i].type = src[i].type;
         }
         break;
      case OpMinus:
         for (i = 0; i < len; i++) {
            if (src[i].type == MincFloatType)
               dest[i].val.number = src[i].val.number - val;
            else
               dest[i].val = src[i].val;
            dest[i].type = src[i].type;
         }
         break;
      case OpMul:
         for (i = 0; i < len; i++) {
            if (src[i].type == MincFloatType)
               dest[i].val.number = src[i].val.number * val;
            else
               dest[i].val = src[i].val;
            dest[i].type = src[i].type;
         }
         break;
      case OpDiv:
         for (i = 0; i < len; i++) {
            if (src[i].type == MincFloatType)
               dest[i].val.number = src[i].val.number / val;
            else
               dest[i].val = src[i].val;
            dest[i].type = src[i].type;
         }
         break;
      case OpMod:
         for (i = 0; i < len; i++) {
            if (src[i].type == MincFloatType)
               dest[i].val.number = (MincFloat) ((long) src[i].val.number
                                                            % (long) val);
            else
               dest[i].val = src[i].val;
            dest[i].type = src[i].type;
         }
         break;
      case OpPow:
         for (i = 0; i < len; i++) {
            if (src[i].type == MincFloatType)
               dest[i].val.number = (MincFloat) pow((double) src[i].val.number,
                                                                  (double) val);
            else
               dest[i].val = src[i].val;
            dest[i].type = src[i].type;
         }
         break;
      case OpNeg:
         for (i = 0; i < len; i++) {
            if (src[i].type == MincFloatType)
               dest[i].val.number = -src[i].val.number;    /* <val> ignored */
            else
               dest[i].val = src[i].val;
            dest[i].type = src[i].type;
         }
         break;
      default:
         for (i = 0; i < len; i++)
            dest[i].val.number = 0.0;
         minc_internal_error("invalid list operator");
         break;
   }
   assert(tp->type == MincVoidType);	// are we ever overwriting these?
   tp->type = MincListType;
   assert(tp->v.list == NULL);
   tp->v.list = destList;
   DPRINT3("list %p refcount %d -> %d\n", destList, destList->refcount, destList->refcount+1);
   ++destList->refcount;
}


/* --------------------------------------------------------- exct_operator -- */
static void
exct_operator(Tree tp, OpKind op)
{
   Tree child0, child1;

   child0 = exct(tp->u.child[0]);
   child1 = exct(tp->u.child[1]);
   switch (child0->type) {
      case MincFloatType:
         switch (child1->type) {
            case MincFloatType:
               do_op_num(tp, child0->v.number, child1->v.number, op);
               break;
            case MincStringType:
               {
                  char buf[64];
                  snprintf(buf, 64, "%g", child0->v.number);
                  do_op_string(tp, buf, child1->v.string, op);
               }
               break;
            case MincHandleType:
               do_op_num_handle(tp, child0->v.number, child1->v.handle, op);
               break;
            case MincListType:
               /* Check for nonsensical ops. */
               if (op == OpMinus)
                  minc_warn("can't subtract a list from a number");
               else if (op == OpDiv)
                  minc_warn("can't divide a number by a list");
               else
                  do_op_list_iterate(tp, child1, child0->v.number, op);
               break;
            default:
               minc_internal_error("exct_operator: invalid type");
               break;
         }
         break;
      case MincStringType:
         switch (child1->type) {
            case MincFloatType:
               {
                  char buf[64];
                  snprintf(buf, 64, "%g", child1->v.number);
                  do_op_string(tp, child0->v.string, buf, op);
               }
               break;
            case MincStringType:
               do_op_string(tp, child0->v.string, child1->v.string, op);
               break;
            case MincHandleType:
               minc_warn("can't operate on a string and a handle");
               break;
            case MincListType:
               minc_warn("can't operate on a string and a list");
               break;
            default:
               minc_internal_error("exct_operator: invalid type");
               break;
         }
         break;
      case MincHandleType:
         switch (child1->type) {
            case MincFloatType:
               do_op_handle_num(tp, child0->v.handle, child1->v.number, op);
               break;
            case MincStringType:
               minc_warn("can't operate on a string and a handle");
               break;
            case MincHandleType:
               do_op_handle_handle(tp, child0->v.handle, child1->v.handle, op);
               break;
            case MincListType:
               minc_warn("can't operate on a list and a handle");
               break;
            default:
               minc_internal_error("exct_operator: invalid type");
               break;
         }
         break;
      case MincListType:
         switch (child1->type) {
            case MincFloatType:
               do_op_list_iterate(tp, child0, child1->v.number, op);
               break;
            case MincStringType:
               minc_warn("can't operate on a string");
               break;
            case MincHandleType:
               minc_warn("can't operate on a handle");
               break;
            case MincListType:
               minc_warn("can't operate on two lists");
               break;
            default:
               minc_internal_error("exct_operator: invalid type");
               break;
         }
         break;
      default:
         minc_internal_error("exct_operator: invalid type");
         break;
   }
}


/* --------------------------------------------------------- exct_relation -- */
static void
exct_relation(Tree tp)
{
   Tree tp0 = exct(tp->u.child[0]);
   Tree tp1 = exct(tp->u.child[1]);

   tp->type = MincFloatType;

   if (tp0->type != tp1->type) {
      minc_warn("attempt to compare objects having different types");
      tp->v.number = 0.0;
      return;
   }

   switch (tp->op) {
      case OpEqual:
         switch (tp0->type) {
            case MincFloatType:
               if (cmp(tp0->v.number, tp1->v.number) == 0)
                  tp->v.number = 1.0;
               else
                  tp->v.number = 0.0;
               break;
            case MincStringType:
               if (strcmp(tp0->v.string, tp1->v.string) == 0)
                  tp->v.number = 1.0;
               else
                  tp->v.number = 0.0;
               break;
            default:
               goto unsupported_type;
               break;
         }
         break;
      case OpNotEqual:
         switch (tp0->type) {
            case MincFloatType:
               if (cmp(tp0->v.number, tp1->v.number) == 0)
                  tp->v.number = 0.0;
               else
                  tp->v.number = 1.0;
               break;
            case MincStringType:
               if (strcmp(tp0->v.string, tp1->v.string) == 0)
                  tp->v.number = 0.0;
               else
                  tp->v.number = 1.0;
               break;
            default:
               goto unsupported_type;
               break;
         }
         break;
      case OpLess:
         switch (tp0->type) {
            case MincFloatType:
               if (cmp(tp0->v.number, tp1->v.number) == -1)
                  tp->v.number = 1.0;
               else
                  tp->v.number = 0.0;
               break;
            default:
               goto unsupported_type;
               break;
         }
         break;
      case OpGreater:
         switch (tp0->type) {
            case MincFloatType:
               if (cmp(tp0->v.number, tp1->v.number) == 1)
                  tp->v.number = 1.0;
               else
                  tp->v.number = 0.0;
               break;
            default:
               goto unsupported_type;
               break;
         }
         break;
      case OpLessEqual:
         switch (tp0->type) {
            case MincFloatType:
               if (cmp(tp0->v.number, tp1->v.number) <= 0)
                  tp->v.number = 1.0;
               else
                  tp->v.number = 0.0;
               break;
            default:
               goto unsupported_type;
               break;
         }
         break;
      case OpGreaterEqual:
         switch (tp0->type) {
            case MincFloatType:
               if (cmp(tp0->v.number, tp1->v.number) >= 0)
                  tp->v.number = 1.0;
               else
                  tp->v.number = 0.0;
               break;
            default:
               goto unsupported_type;
               break;
         }
         break;
      default:
         minc_internal_error("exct: tried to execute invalid NodeRelation");
         break;
   }
   return;
unsupported_type:
   minc_internal_error("can't compare this type of object");
}


/* --------------------------------------------------------- exct_opassign -- */
static void
exct_opassign(Tree tp, OpKind op)
{
   Tree tp0 = tp->u.child[0];
   Tree tp1 = exct(tp->u.child[1]);

   if (tp0->u.symbol->type != MincFloatType || tp1->type != MincFloatType) {
      minc_warn("can only use '%c=' with numbers",
                           op == OpPlus ? '+' : (op == OpMinus ? '-'
                                              : (op == OpMul ? '*' : '/')));
//FIXME: Is this correct?
//      memcpy(&tp->v, &tp0->u.symbol->v, sizeof(MincValue));
//      tp->type = tp0->type;
      copy_sym_tree(tp, tp0->u.symbol);
      return;
   }

   switch (tp->op) {
      case OpPlus:
         tp0->u.symbol->v.number += tp1->v.number;
         break;
      case OpMinus:
         tp0->u.symbol->v.number -= tp1->v.number;
         break;
      case OpMul:
         tp0->u.symbol->v.number *= tp1->v.number;
         break;
      case OpDiv:
         tp0->u.symbol->v.number /= tp1->v.number;
         break;
      default:
         minc_internal_error("exct: tried to execute invalid NodeOpAssign");
         break;
   }
   tp0->u.symbol->type = tp1->type;
   tp->v.number = tp0->u.symbol->v.number;
   tp->type = tp1->type;
}


/* --------------------------------------------------- exct_subscript_read -- */
static void
exct_subscript_read(Tree tp)
{
   exct(tp->u.child[1]);
   if (tp->u.child[1]->type == MincFloatType) {
      if (tp->u.child[0]->u.symbol->type == MincListType) {
         MincListElem elem;
         MincFloat fltindex = tp->u.child[1]->v.number;
         int index = (int) fltindex;
         MincFloat frac = fltindex - index;
		 MincList *theList = tp->u.child[0]->u.symbol->v.list;
		 int len = 0;
         if (theList == NULL)
            minc_die("attempt to index an empty list");
         len = theList->len;
         if (fltindex < 0.0) {    /* -1 means last element */
            if (fltindex <= -2.0)
               minc_warn("negative index ... returning last element");
            index = len - 1;
            fltindex = (MincFloat) index;
         }
         else if (fltindex > (MincFloat) (len - 1)) {
            minc_warn("attempt to index past the end of list ... "
                                                "returning last element");
            index = len - 1;
            fltindex = (MincFloat) index;
         }
		 elem.type = MincVoidType;
		 copy_listelem_elem(&elem, &theList->data[index]);		 	

         /* do linear interpolation for float items */
         if (elem.type == MincFloatType && frac > 0.0 && index < len - 1) {
            MincListElem elem2 = theList->data[index + 1];
            if (elem2.type == MincFloatType)
               tp->v.number = elem.val.number
                        + (frac * (elem2.val.number - elem.val.number));
            else  /* can't interpolate btw. a number and another type */
               tp->v.number = elem.val.number;
            tp->type = elem.type;
         }
         else {
            copy_listelem_tree(tp, &elem);
         }
		 clear_elem(&elem);
      }
      else
         minc_die("attempt to index a variable that's not a list");
   }
   else
      minc_die("list index must be a number");
}


/* -------------------------------------------------- exct_subscript_write -- */
static void
exct_subscript_write(Tree tp)
{
   exct(tp->u.child[1]);         /* index */
   exct(tp->u.child[2]);         /* expression to store */
   if (tp->u.child[1]->type == MincFloatType) {
      if (tp->u.child[0]->u.symbol->type == MincListType) {
         int len = 0;
		 MincList *theList = tp->u.child[0]->u.symbol->v.list;
         MincFloat fltindex = tp->u.child[1]->v.number;
         int index = (int) fltindex;
         if (fltindex - (MincFloat) index > 0.0)
            minc_warn("list index must be integer ... correcting");
		 if (theList != NULL) {
        	len = theList->len;
        	assert(len >= 0);    /* NB: okay to have zero-length list */
		 }
         if (index == -1)     /* means last element */
            index = len > 0 ? len - 1 : 0;
         else if (index >= len) {
            /* resize list */
            int i, newslots, oldlen = len;
            newslots = len > 0 ? (index - (len - 1)) : index + 1;
            len += newslots;
			if (len < 0) {
			    minc_die("list array subscript exceeds integer size limit!");
			}
			if (theList == NULL)
				tp->u.child[0]->u.symbol->v.list = theList = newList(len);
			else
				resizeList(theList, len);
			DPRINT2("exct_subscript_write: MincList %p expanded to len %d\n",
					theList->data, len);
			// Ref the list if just allocated.
			if (theList->refcount == 0)
				theList->refcount = 1;
  			 DPRINT1("list %p refcount = 1\n", theList);
         }
         copy_tree_listelem(&theList->data[index], tp->u.child[2]);
		 assert(theList->data[index].type == tp->u.child[2]->type);
         copy_tree_tree(tp, tp->u.child[2]);
      }
      else
         minc_die("attempt to index a variable that's not a list");
   }
   else
      minc_die("list index must be a number");
}


/* ========================================================================== */
/* Tree execution and disposal */

/* ------------------------------------------------------ check_list_count -- */
/* This protects us against a situation that can arise due to our use of
   '{' and '}' to delimit both statements and list contents.  If you write 
   the following in a script, it will quickly chew through all available
   memory, as it allocates a zero-length block for an empty list on each
   iteration.

      while (1) {}

   This function prevents this from going on for too many thousands of
   iterations.
*/
#define MAX_LISTS 50000

static int
check_list_count()
{
   static int list_count = 0;
   if (++list_count > MAX_LISTS) {
      minc_die("Bailing out due to suspected infinite loop on "
               "empty code block\n(e.g., \"while (1) {}\").");
      return -1;
   }
   return 0;
}


/* ------------------------------------------------------------------ exct -- */
/* This recursive function interprets the intermediate code.
*/
Tree
exct(Tree tp)
{
   if (tp == NULL)
      return NULL;

   switch (tp->kind) {
      case NodeConstf:
         DPRINT1("exct (enter NodeConstf, tp=%p)\n", tp);
         tp->type = MincFloatType;
         tp->v.number = tp->u.number;
         DPRINT1("exct (exit NodeConstf, tp=%p)\n", tp);
         break;
      case NodeString:
         DPRINT1("exct (enter NodeString, tp=%p)\n", tp);
         tp->type = MincStringType;
         tp->v.string = tp->u.string;
         DPRINT1("exct (exit NodeString, tp=%p)\n", tp);
         break;
      case NodeName:
         DPRINT1("exct (enter NodeName, tp=%p)\n", tp);
         /* assign what's in the symbol into tree's value field */
         copy_sym_tree(tp, tp->u.symbol);
		 assert(tp->type == tp->u.symbol->type);
         DPRINT1("exct (exit NodeName, tp=%p)\n", tp);
         break;
      case NodeListElem:
         DPRINT1("exct (enter NodeListElem, tp=%p)\n", tp);
         exct(tp->u.child[0]);
         if (list_len == MAXDISPARGS)
            minc_die("exceeded maximum number of items for a list");
         {
            Tree tmp = exct(tp->u.child[1]);
            /* Copy entire MincValue union from expr to tp and to stack. */
            copy_tree_tree(tp, tmp);
            copy_tree_listelem(&list[list_len], tmp);
			assert(list[list_len].type == tmp->type);
            list_len++;
         }
         DPRINT1("exct (exit NodeListElem, tp=%p)\n", tp);
         break;
      case NodeEmptyListElem:
         DPRINT1("exct (enter NodeEmptyListElem, tp=%p)\n", tp);
         /* do nothing */
         DPRINT("exct (exit NodeEmptyListElem)\n");
         break;
      case NodeList:
         DPRINT1("exct (enter NodeList, tp=%p)\n", tp);
         push_list();
         exct(tp->u.child[0]);     /* NB: increments list_len */
         {
		    MincList *theList;
			int i;
            if (check_list_count() < 0)
               return tp;
			theList = newList(list_len);
            if (theList == NULL)
               return NULL;
			if (tp->type == MincListType && tp->v.list != NULL)
			   unref_value_list(&tp->v);
            tp->type = MincListType;
            tp->v.list = theList;
  			DPRINT1("list assigned to tree %p\n", tp);
			theList->refcount = 1;
  			DPRINT1("list %p refcount = 1\n", theList);
			// Copy from stack list into tree list.
			for (i = 0; i < list_len; ++i)
            	copy_listelem_elem(&theList->data[i], &list[i]);
         }
         pop_list();
         DPRINT1("exct (exit NodeList, tp=%p)\n", tp);
         break;
      case NodeSubscriptRead:
         DPRINT1("exct (enter NodeSubscriptRead, tp=%p)\n", tp);
         exct_subscript_read(tp);
         DPRINT1("exct (exit NodeSubscriptRead, tp=%p)\n", tp);
         break;
      case NodeSubscriptWrite:
         DPRINT1("exct (enter NodeSubscriptWrite, tp=%p)\n", tp);
         exct_subscript_write(tp);
         DPRINT1("exct (exit NodeSubscriptWrite, tp=%p)\n", tp);
         break;
      case NodeCall:
         DPRINT1("exct (enter NodeCall, tp=%p)\n", tp);
         push_list();
         exct(tp->u.child[0]);
         {
            MincListElem retval;
            int result = call_builtin_function(tp->funcname, list, list_len,
                                                                     &retval);
            if (result < 0)
               result = call_external_function(tp->funcname, list, list_len,
                                                                     &retval);
              copy_listelem_tree(tp, &retval);
			  assert(tp->type == retval.type);
			  clear_elem(&retval);
         }
         pop_list();
         DPRINT1("exct (exit NodeCall, tp=%p)\n", tp);
         break;
      case NodeStore:
         DPRINT1("exct (enter NodeStore, tp=%p)\n", tp);
         /* Store value and type into sym pointed to by child[0]->u.symbol. */
         exct(tp->u.child[1]);
         /* Copy entire MincValue union from expr to id sym and to tp. */
         copy_tree_sym(tp->u.child[0]->u.symbol, tp->u.child[1]);
		 assert(tp->u.child[0]->u.symbol->type == tp->u.child[1]->type);
         copy_tree_tree(tp, tp->u.child[1]);
         assert(tp->type == tp->u.child[1]->type);
         DPRINT2("exct (exit NodeStore, tp=%p, type=%d)\n", tp, tp->type);
         break;
      case NodeOpAssign:
         DPRINT1("exct (enter NodeOpAssign, tp=%p)\n", tp);
         exct_opassign(tp, tp->op);
         DPRINT1("exct (exit NodeOpAssign, tp=%p)\n", tp);
         break;
      case NodeNot:
         DPRINT1("exct (enter NodeNot, tp=%p)\n", tp);
         tp->type = MincFloatType;
         if (cmp(0.0, exct(tp->u.child[0])->v.number) == 0)
            tp->v.number = 1.0;
         else
            tp->v.number = 0.0;
         DPRINT1("exct (exit NodeNot, tp=%p)\n", tp);
         break;
      case NodeAnd:
         DPRINT1("exct (enter NodeAnd, tp=%p)\n", tp);
         tp->type = MincFloatType;
         tp->v.number = 0.0;
         if (cmp(0.0, exct(tp->u.child[0])->v.number) != 0) {
            if (cmp(0.0, exct(tp->u.child[1])->v.number) != 0) {
               tp->type = MincFloatType;
               tp->v.number = 1.0;
            }
         }
         DPRINT1("exct (exit NodeAnd, tp=%p)\n", tp);
         break;
      case NodeRelation:
         DPRINT2("exct (enter NodeRelation, tp=%p, op=%d)\n", tp, tp->op);
         exct_relation(tp);
         DPRINT2("exct (exit NodeRelation, tp=%p, op=%d)\n", tp, tp->op);
         break; /* switch NodeRelation */
      case NodeOperator:
         DPRINT2("exct (enter NodeOperator, tp=%p, op=%d)\n", tp, tp->op);
         exct_operator(tp, tp->op);
         DPRINT2("exct (exit NodeOperator, tp=%p, op=%d)\n", tp, tp->op);
         break; /* switch NodeOperator */
      case NodeUnaryOperator:
         DPRINT1("exct (enter NodeUnaryOperator, tp=%p)\n", tp);
         tp->type = MincFloatType;
         if (tp->op == OpNeg)
            tp->v.number = -exct(tp->u.child[0])->v.number;
         DPRINT1("exct (exit NodeUnaryOperator, tp=%p)\n", tp);
         break;
      case NodeOr:
         DPRINT1("exct (enter NodeOr, tp=%p)\n", tp);
         tp->type = MincFloatType;
         tp->v.number = 0.0;
         if ((cmp(0.0, exct(tp->u.child[0])->v.number) != 0) ||
             (cmp(0.0, exct(tp->u.child[1])->v.number) != 0)) {
            tp->v.number = 1.0;
         }
         DPRINT1("exct (exit NodeOr, tp=%p)\n", tp);
         break;
      case NodeIf:
         DPRINT1("exct (enter NodeIf, tp=%p)\n", tp);
         if (cmp(0.0, exct(tp->u.child[0])->v.number) != 0)
            exct(tp->u.child[1]);
         DPRINT1("exct (exit NodeIf, tp=%p)\n", tp);
         break;
      case NodeIfElse:
         DPRINT1("exct (enter NodeIfElse, tp=%p)\n", tp);
         if (cmp(0.0, exct(tp->u.child[0])->v.number) != 0)
            exct(tp->u.child[1]);
         else
            exct(tp->u.child[2]);
         DPRINT1("exct (exit NodeIfElse, tp=%p)\n", tp);
         break;
      case NodeWhile:
         DPRINT1("exct (enter NodeWhile, tp=%p)\n", tp);
         while (cmp(0.0, exct(tp->u.child[0])->v.number) != 0)
            exct(tp->u.child[1]);
         DPRINT1("exct (exit NodeWhile, tp=%p)\n", tp);
         break;
      case NodeNoop:
         DPRINT1("exct (enter NodeNoop, tp=%p)\n", tp);
         /* do nothing */
         DPRINT("exct (exit NodeNoop)\n");
         break;
      case NodeFor:
         DPRINT1("exct (enter NodeFor, tp=%p)\n", tp);
         exct(tp->u.child[0]);         /* init */
         while (cmp(0.0, exct(tp->u.child[1])->v.number) != 0) { /* condition */
            exct(tp->u.child[3]);      /* execute block */
            exct(tp->u.child[2]);      /* prepare for next iteration */
         }
         DPRINT1("exct (exit NodeFor, tp=%p)\n", tp);
         break;
      case NodeSeq:
         DPRINT1("exct (enter NodeSeq, tp=%p)\n", tp);
         exct(tp->u.child[0]);
         exct(tp->u.child[1]);
         DPRINT1("exct (exit NodeSeq, tp=%p)\n", tp);
         break;
      default:
         minc_internal_error("exct: tried to execute invalid node");
         break;
   } /* switch kind */

   return tp;
}

static void
clear_list(MincListElem *list, int len)
{
	int i;
	for (i = 0; i < len; ++i) {
		clear_elem(&list[i]);
	}
}

static void
push_list()
{
   DPRINT("push_list\n");
   if (list_stack_ptr >= MAXSTACK)
      minc_die("stack overflow: too many nested function calls");
   list_stack[list_stack_ptr] = list;
   list_len_stack[list_stack_ptr++] = list_len;
   list = (MincListElem *) calloc(MAXDISPARGS, sizeof(MincListElem));
   DPRINT1("push_list calloc: list=%p\n", list);
   list_len = 0;
}


static void
pop_list()
{
   DPRINT("pop_list\n");
   DPRINT1("pop_list free: list=%p\n", list);
   clear_list(list, MAXDISPARGS);
   efree(list);
   if (list_stack_ptr == 0)
      minc_die("stack underflow");
   list = list_stack[--list_stack_ptr];
   list_len = list_len_stack[list_stack_ptr];
}

static void copy_value(MincValue *dest, MincDataType destType,
                       MincValue *src, MincDataType srcType)
{
//   assert(srcType != MincVoidType);
   if (srcType == MincHandleType && src->handle != NULL) {
      ref_handle(src->handle);	// ref before unref
   }
   else if (srcType == MincListType) {
      DPRINT3("list %p refcount %d -> %d\n", src->list, src->list->refcount, src->list->refcount+1);
      ++src->list->refcount;
   }
   if (destType == MincHandleType && dest->handle != NULL) {
#ifdef DEBUG
      DPRINT("\toverwriting existing RtcHandle value\n");
#endif
      unref_handle(dest->handle);	// overwriting handle, so unref
   }
   else if (destType == MincListType) {
#ifdef DEBUG
      DPRINT("\toverwriting existing MincList value\n");
#endif
      unref_value_list(dest);
   }
   memcpy(dest, src, sizeof(MincValue));
}

/* This copies a Tree node's value and handles ref counting when necessary */
static void
copy_tree_tree(Tree tpdest, Tree tpsrc)
{
   int no_ref = 0;
   DPRINT2("copy_tree_tree(%p, %p)\n", tpdest, tpsrc);
   copy_value(&tpdest->v, tpdest->type, &tpsrc->v, tpsrc->type);
   tpdest->type = tpsrc->type;
}

/* This copies a Symbol's value and handles ref counting when necessary */
static void
copy_sym_tree(Tree tpdest, Symbol *src)
{
   int no_ref = 0;
   DPRINT2("copy_sym_tree(%p, %p)\n", tpdest, src);
   copy_value(&tpdest->v, tpdest->type, &src->v, src->type);
   tpdest->type = src->type;
}

static void
copy_tree_sym(Symbol *dest, Tree tpsrc)
{
   int no_ref = 0;
   DPRINT2("copy_tree_sym(%p, %p)\n", dest, tpsrc);
   copy_value(&dest->v, dest->type, &tpsrc->v, tpsrc->type);
   dest->type = tpsrc->type;
}

static void
copy_tree_listelem(MincListElem *dest, Tree tpsrc)
{
   int no_ref = 0;
   DPRINT2("copy_tree_listelem(%p, %p)\n", dest, tpsrc);
   copy_value(&dest->val, dest->type, &tpsrc->v, tpsrc->type);
   dest->type = tpsrc->type;
}

static void
copy_listelem_tree(Tree tpdest, MincListElem *esrc)
{
   int no_ref = 0;
   DPRINT2("copy_listelem_tree(%p, %p)\n", tpdest, esrc);
   copy_value(&tpdest->v, tpdest->type, &esrc->val, esrc->type);
   tpdest->type = esrc->type;
}

static void
copy_listelem_elem(MincListElem *edest, MincListElem *esrc)
{
   int no_ref = 0;
   DPRINT2("copy_listelem_elem(%p, %p)\n", edest, esrc);
   copy_value(&edest->val, edest->type, &esrc->val, esrc->type);
   edest->type = esrc->type;
}

/* This recursive function frees space. */
void
free_tree(Tree tp)
{
   if (tp == NULL)
      return;

   DPRINT2("entering free_tree(%p) (%s)\n", tp, printNodeKind(tp->kind));

   switch (tp->kind) {
      case NodeZero:
         break;
      case NodeSeq:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         break;
      case NodeStore:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         break;
      case NodeList:
         free_tree(tp->u.child[0]);
         break;
      case NodeListElem:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         break;
      case NodeEmptyListElem:
         break;
      case NodeSubscriptRead:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         break;
      case NodeSubscriptWrite:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         free_tree(tp->u.child[2]);
         break;
      case NodeOpAssign:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         break;
      case NodeName:
         break;
      case NodeString:
         break;
      case NodeConstf:
         break;
      case NodeCall:
         free_tree(tp->u.child[0]);
         break;
      case NodeNot:
         free_tree(tp->u.child[0]);
         break;
      case NodeAnd:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         break;
      case NodeRelation:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         break;
      case NodeOperator:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         break;
      case NodeUnaryOperator:
         free_tree(tp->u.child[0]);
         break;
      case NodeOr:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         break;
      case NodeIf:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         break;
      case NodeIfElse:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         free_tree(tp->u.child[2]);
         break;
      case NodeWhile:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         break;
      case NodeNoop:
         break;
      case NodeFor:
         free_tree(tp->u.child[0]);
         free_tree(tp->u.child[1]);
         free_tree(tp->u.child[2]);
         free_tree(tp->u.child[3]);
         break;
   } /* switch kind */

   if (tp->type == MincHandleType) {
      unref_handle(tp->v.handle);
   }
   else if (tp->type == MincListType) {
      unref_value_list(&tp->v);
   }
   tp->type = MincVoidType;		// To prevent double-free
   efree(tp);   /* actually free space */
#ifdef DEBUG
	--numTrees;
   DPRINT1("[%d trees left]\n", numTrees);
#endif
   DPRINT1("leaving free_tree(%p)\n", tp);
}

void
clear_elem(MincListElem *elem)
{
	if (elem->type == MincListType) {
	   DPRINT1("clear_elem(%p)\n", elem);
       unref_value_list(&elem->val);
	}
	else if (elem->type == MincHandleType) {
	   DPRINT1("clear_elem(%p)\n", elem);
	   unref_handle(elem->val.handle);
	}
}


void
unref_value_list(MincValue *value)
{
   DPRINT3("unref_value_list(%p) [%d -> %d]\n", value->list, value->list->refcount, value->list->refcount-1);
   assert(value->list->refcount > 0);
   if (--value->list->refcount == 0) {
      if (value->list->data != NULL) {
		 int e;
		 DPRINT1("\tfreeing MincList data %p...\n", value->list->data);
		 for (e = 0; e < value->list->len; ++e) {
			 MincListElem *elem = &value->list->data[e];
			 clear_elem(elem);
		 }
		 efree(value->list->data);
		 value->list->data = NULL;
		 DPRINT("\tdone\n");
      }
	  DPRINT1("\tfreeing MincList %p\n", value->list);
	  efree(value->list);
   }
}

