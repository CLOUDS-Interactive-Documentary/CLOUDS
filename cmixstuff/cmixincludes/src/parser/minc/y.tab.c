/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LOWPRIO = 258,
     TOK_MINUSEQU = 259,
     TOK_PLUSEQU = 260,
     TOK_DIVEQU = 261,
     TOK_MULEQU = 262,
     TOK_OR = 263,
     TOK_AND = 264,
     TOK_UNEQU = 265,
     TOK_EQU = 266,
     TOK_GTREQU = 267,
     TOK_LESSEQU = 268,
     TOK_POW = 269,
     CASTTOKEN = 270,
     TOK_FLOAT_DECL = 271,
     TOK_STRING_DECL = 272,
     TOK_HANDLE_DECL = 273,
     TOK_IDENT = 274,
     TOK_NUM = 275,
     TOK_NOT = 276,
     TOK_IF = 277,
     TOK_ELSE = 278,
     TOK_FOR = 279,
     TOK_WHILE = 280,
     TOK_TRUE = 281,
     TOK_FALSE = 282,
     TOK_STRING = 283
   };
#endif
/* Tokens.  */
#define LOWPRIO 258
#define TOK_MINUSEQU 259
#define TOK_PLUSEQU 260
#define TOK_DIVEQU 261
#define TOK_MULEQU 262
#define TOK_OR 263
#define TOK_AND 264
#define TOK_UNEQU 265
#define TOK_EQU 266
#define TOK_GTREQU 267
#define TOK_LESSEQU 268
#define TOK_POW 269
#define CASTTOKEN 270
#define TOK_FLOAT_DECL 271
#define TOK_STRING_DECL 272
#define TOK_HANDLE_DECL 273
#define TOK_IDENT 274
#define TOK_NUM 275
#define TOK_NOT 276
#define TOK_IF 277
#define TOK_ELSE 278
#define TOK_FOR 279
#define TOK_WHILE 280
#define TOK_TRUE 281
#define TOK_FALSE 282
#define TOK_STRING 283




/* Copy the first part of user declarations.  */
#line 5 "minc.y"

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


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 188 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  43
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   342

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  45
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  12
/* YYNRULES -- Number of rules.  */
#define YYNRULES  68
/* YYNRULES -- Number of states.  */
#define YYNSTATES  123

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   283

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    44,     2,     2,
      37,    38,    19,    17,    43,    18,     2,    20,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    36,
      13,     4,    14,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    41,     2,    42,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    39,     2,    40,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     5,
       6,     7,     8,     9,    10,    11,    12,    15,    16,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    11,    14,    18,    20,
      23,    26,    29,    34,    41,    46,    57,    61,    64,    67,
      70,    73,    76,    79,    82,    85,    88,    89,    93,    97,
     101,   105,   109,   114,   118,   123,   130,   132,   136,   138,
     140,   144,   145,   147,   149,   152,   156,   160,   164,   168,
     172,   176,   180,   184,   186,   188,   190,   194,   198,   202,
     206,   210,   214,   218,   220,   222,   224,   226,   229
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      46,     0,    -1,    -1,    47,    -1,    48,    -1,    48,    36,
      -1,    47,    48,    -1,    47,    48,    36,    -1,    50,    -1,
      23,    51,    -1,    24,    51,    -1,    25,    51,    -1,    29,
      49,    55,    48,    -1,    29,    49,    55,    48,    30,    48,
      -1,    32,    49,    55,    48,    -1,    31,    49,    37,    48,
      36,    55,    36,    48,    38,    48,    -1,    39,    47,    40,
      -1,     1,    23,    -1,     1,    24,    -1,     1,    25,    -1,
       1,    29,    -1,     1,    32,    -1,     1,    31,    -1,     1,
      39,    -1,     1,    30,    -1,     1,    36,    -1,    -1,    52,
       4,    56,    -1,    52,     6,    56,    -1,    52,     5,    56,
      -1,    52,     8,    56,    -1,    52,     7,    56,    -1,    52,
      37,    53,    38,    -1,    39,    53,    40,    -1,    52,    41,
      56,    42,    -1,    52,    41,    56,    42,     4,    56,    -1,
      52,    -1,    51,    43,    52,    -1,    26,    -1,    56,    -1,
      53,    43,    56,    -1,    -1,    35,    -1,    56,    -1,    28,
      55,    -1,    55,    10,    55,    -1,    55,     9,    55,    -1,
      55,    12,    55,    -1,    56,    11,    56,    -1,    56,    13,
      56,    -1,    56,    14,    56,    -1,    56,    16,    56,    -1,
      56,    15,    56,    -1,    33,    -1,    34,    -1,    50,    -1,
      56,    21,    56,    -1,    56,    19,    56,    -1,    56,    20,
      56,    -1,    56,    17,    56,    -1,    56,    18,    56,    -1,
      56,    44,    56,    -1,    37,    55,    38,    -1,    54,    -1,
      27,    -1,    33,    -1,    34,    -1,    18,    56,    -1,    52,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    55,    55,    55,    59,    60,    61,    62,    66,    72,
      73,    74,    75,    79,    83,    87,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   104,   108,   114,   123,
     132,   141,   151,   154,   156,   165,   177,   178,   182,   186,
     187,   192,   196,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   232,   233,   234,   238
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "LOWPRIO", "'='", "TOK_MINUSEQU",
  "TOK_PLUSEQU", "TOK_DIVEQU", "TOK_MULEQU", "TOK_OR", "TOK_AND",
  "TOK_UNEQU", "TOK_EQU", "'<'", "'>'", "TOK_GTREQU", "TOK_LESSEQU", "'+'",
  "'-'", "'*'", "'/'", "TOK_POW", "CASTTOKEN", "TOK_FLOAT_DECL",
  "TOK_STRING_DECL", "TOK_HANDLE_DECL", "TOK_IDENT", "TOK_NUM", "TOK_NOT",
  "TOK_IF", "TOK_ELSE", "TOK_FOR", "TOK_WHILE", "TOK_TRUE", "TOK_FALSE",
  "TOK_STRING", "';'", "'('", "')'", "'{'", "'}'", "'['", "']'", "','",
  "'%'", "$accept", "prg", "stml", "stmt", "level", "rstmt", "idl", "id",
  "expl", "str", "bexp", "exp", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,    61,   259,   260,   261,   262,   263,
     264,   265,   266,    60,    62,   267,   268,    43,    45,    42,
      47,   269,   270,   271,   272,   273,   274,   275,   276,   277,
     278,   279,   280,   281,   282,   283,    59,    40,    41,   123,
     125,    91,    93,    44,    37
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    45,    46,    46,    47,    47,    47,    47,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    48,    48,    48,    49,    50,    50,    50,
      50,    50,    50,    50,    50,    50,    51,    51,    52,    53,
      53,    53,    54,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     2,     2,     3,     1,     2,
       2,     2,     4,     6,     4,    10,     3,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     0,     3,     3,     3,
       3,     3,     4,     3,     4,     6,     1,     3,     1,     1,
       3,     0,     1,     1,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     1,     1,     1,     1,     2,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,    38,    26,    26,    26,     0,
       0,     0,     4,     8,     0,    17,    18,    19,    20,    24,
      22,    21,    25,    23,     9,    36,    10,    11,     0,     0,
       0,     0,    64,    65,    66,    42,     0,     0,     8,    68,
       0,    63,    39,     1,     6,     5,     0,     0,     0,     0,
       0,    41,     0,     0,     0,    53,    54,    41,    55,     0,
      43,     0,     0,    67,     0,    16,    33,     0,     0,     0,
       0,     0,     0,     0,     7,    27,    29,    28,    31,    30,
       0,     0,    37,    44,     0,     0,     0,    12,     0,     0,
       0,     0,     0,     0,    14,    62,    40,    59,    60,    57,
      58,    56,    61,    32,    34,    46,    45,    47,     0,    48,
      49,    50,    52,    51,     0,     0,    13,     0,    35,     0,
       0,     0,    15
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    10,    11,    12,    28,    58,    24,    39,    40,    41,
      59,    60
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -22
static const yytype_int16 yypact[] =
{
       9,   303,   -14,   -14,   -14,   -22,   -22,   -22,   -22,    54,
      25,   135,    -7,   -22,   209,   -22,   -22,   -22,   -22,   -22,
     -22,   -22,   -22,   -22,     2,   -22,     2,     2,   268,    10,
     268,   286,   -22,   -22,   -22,   -22,   268,   154,    97,   209,
     -21,   -22,   271,   -22,    24,   -22,   286,   286,   286,   286,
     286,   286,   286,   -14,   268,   224,   238,   286,   -22,   180,
     256,   197,   180,    21,     8,   -22,   -22,   286,   286,   286,
     286,   286,   286,   286,   -22,   271,   271,   271,   271,   271,
      20,   243,   -22,   -22,   268,   268,   268,    38,   286,   286,
     286,   286,   286,    37,   -22,   -22,   271,   -13,   -13,    -8,
      -8,    21,   271,   -22,    50,    18,    62,   -22,   197,   271,
     271,   271,   271,   271,   268,   286,   -22,    14,   271,   197,
      44,   197,   -22
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -22,   -22,    66,   -10,    36,     5,    53,     0,    33,   -22,
     -15,    81
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -67
static const yytype_int8 yytable[] =
{
      14,    44,    25,    25,    25,    13,    70,    71,    72,    -2,
       1,    14,     5,    72,    38,    62,    13,    84,    85,    66,
      86,    64,    67,    84,    85,    43,    86,    44,    85,    45,
      86,    73,     2,     3,     4,     5,    73,    14,     6,    83,
       7,     8,    13,    29,    30,    53,    95,    61,     9,    87,
     119,    93,    94,    82,   115,     1,    26,    27,   103,    14,
      74,    14,    14,    67,    13,    73,    13,    13,   108,   105,
     106,   107,    31,   114,    86,    37,     0,     2,     3,     4,
       5,    32,   121,     6,    80,     7,     8,    33,    34,    35,
      42,    36,     0,     9,   -41,     0,     0,   -41,   116,   117,
       0,     0,     0,     0,     0,     0,     0,     0,    14,   120,
       0,   122,    63,    13,   -55,   -55,   -55,   -55,   -55,    14,
       0,    14,     0,     0,    13,     0,    13,    75,    76,    77,
      78,    79,    42,    81,     0,    -3,     1,     0,    42,     0,
     -55,   -55,     0,     0,     0,     0,     0,     0,    96,    97,
      98,    99,   100,   101,   102,     1,     0,     0,     2,     3,
       4,     5,     0,     0,     6,     0,     7,     8,     0,   109,
     110,   111,   112,   113,     9,     0,     0,     2,     3,     4,
       5,     1,     0,     6,     0,     7,     8,     0,     0,    84,
      85,     0,    86,     9,    65,     0,   118,     0,     1,     0,
       0,     0,     0,     2,     3,     4,     5,     0,     0,     6,
       0,     7,     8,    46,    47,    48,    49,    50,     0,     9,
       2,     3,     4,     5,     0,     0,     6,     0,     7,     8,
       0,     0,     0,     0,     0,   -65,     9,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,    51,     0,     0,   -66,
      52,   -66,   -66,   -66,   -66,   -66,   -66,   -66,   -66,   -66,
      68,    69,    70,    71,    72,     0,     0,    88,   -65,    89,
      90,    91,    92,    68,    69,    70,    71,    72,     0,     0,
       0,     0,   -66,     0,     0,   104,    31,    73,    68,    69,
      70,    71,    72,     0,     5,    32,    54,     0,     0,     0,
      73,    55,    56,    35,    31,    36,     0,    57,     0,     0,
       0,     0,     5,    32,     0,    73,     0,     0,     0,    33,
      34,    35,     0,    36,     0,    57,    15,    16,    17,     0,
       0,     0,    18,    19,    20,    21,     0,     0,     0,    22,
       0,     0,    23
};

static const yytype_int8 yycheck[] =
{
       0,    11,     2,     3,     4,     0,    19,    20,    21,     0,
       1,    11,    26,    21,     9,    30,    11,     9,    10,    40,
      12,    36,    43,     9,    10,     0,    12,    37,    10,    36,
      12,    44,    23,    24,    25,    26,    44,    37,    29,    54,
      31,    32,    37,     7,     8,    43,    38,    37,    39,    59,
      36,    61,    62,    53,     4,     1,     3,     4,    38,    59,
      36,    61,    62,    43,    59,    44,    61,    62,    30,    84,
      85,    86,    18,    36,    12,     9,    -1,    23,    24,    25,
      26,    27,    38,    29,    51,    31,    32,    33,    34,    35,
       9,    37,    -1,    39,    40,    -1,    -1,    43,   108,   114,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   119,
      -1,   121,    31,   108,    17,    18,    19,    20,    21,   119,
      -1,   121,    -1,    -1,   119,    -1,   121,    46,    47,    48,
      49,    50,    51,    52,    -1,     0,     1,    -1,    57,    -1,
      43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,
      69,    70,    71,    72,    73,     1,    -1,    -1,    23,    24,
      25,    26,    -1,    -1,    29,    -1,    31,    32,    -1,    88,
      89,    90,    91,    92,    39,    -1,    -1,    23,    24,    25,
      26,     1,    -1,    29,    -1,    31,    32,    -1,    -1,     9,
      10,    -1,    12,    39,    40,    -1,   115,    -1,     1,    -1,
      -1,    -1,    -1,    23,    24,    25,    26,    -1,    -1,    29,
      -1,    31,    32,     4,     5,     6,     7,     8,    -1,    39,
      23,    24,    25,    26,    -1,    -1,    29,    -1,    31,    32,
      -1,    -1,    -1,    -1,    -1,    11,    39,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    37,    -1,    -1,    11,
      41,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      17,    18,    19,    20,    21,    -1,    -1,    11,    44,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    -1,    -1,
      -1,    -1,    44,    -1,    -1,    42,    18,    44,    17,    18,
      19,    20,    21,    -1,    26,    27,    28,    -1,    -1,    -1,
      44,    33,    34,    35,    18,    37,    -1,    39,    -1,    -1,
      -1,    -1,    26,    27,    -1,    44,    -1,    -1,    -1,    33,
      34,    35,    -1,    37,    -1,    39,    23,    24,    25,    -1,
      -1,    -1,    29,    30,    31,    32,    -1,    -1,    -1,    36,
      -1,    -1,    39
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,    23,    24,    25,    26,    29,    31,    32,    39,
      46,    47,    48,    50,    52,    23,    24,    25,    29,    30,
      31,    32,    36,    39,    51,    52,    51,    51,    49,    49,
      49,    18,    27,    33,    34,    35,    37,    47,    50,    52,
      53,    54,    56,     0,    48,    36,     4,     5,     6,     7,
       8,    37,    41,    43,    28,    33,    34,    39,    50,    55,
      56,    37,    55,    56,    55,    40,    40,    43,    17,    18,
      19,    20,    21,    44,    36,    56,    56,    56,    56,    56,
      53,    56,    52,    55,     9,    10,    12,    48,    11,    13,
      14,    15,    16,    48,    48,    38,    56,    56,    56,    56,
      56,    56,    56,    38,    42,    55,    55,    55,    30,    56,
      56,    56,    56,    56,    36,     4,    48,    55,    56,    36,
      48,    38,    48
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 55 "minc.y"
    { MPRINT("prg:"); program = (yyvsp[(1) - (1)].trees); cleanup(); return 0; }
    break;

  case 4:
#line 59 "minc.y"
    { MPRINT("<stmt>"); (yyval.trees) = (yyvsp[(1) - (1)].trees); }
    break;

  case 5:
#line 60 "minc.y"
    { MPRINT("<stmt;>"); (yyval.trees) = (yyvsp[(1) - (2)].trees); }
    break;

  case 6:
#line 61 "minc.y"
    { MPRINT("<stml stmt>"); (yyval.trees) = tseq((yyvsp[(1) - (2)].trees), (yyvsp[(2) - (2)].trees)); }
    break;

  case 7:
#line 62 "minc.y"
    { MPRINT("<stml stmt;>"); (yyval.trees) = tseq((yyvsp[(1) - (3)].trees), (yyvsp[(2) - (3)].trees)); }
    break;

  case 8:
#line 66 "minc.y"
    { MPRINT("<rstmt>");
								if (level == 0) 
									(yyval.trees) = go((yyvsp[(1) - (1)].trees)); 
								else
									(yyval.trees) = (yyvsp[(1) - (1)].trees);
							}
    break;

  case 9:
#line 72 "minc.y"
    { declare(MincFloatType); idcount = 0; }
    break;

  case 10:
#line 73 "minc.y"
    { declare(MincStringType); idcount = 0; }
    break;

  case 11:
#line 74 "minc.y"
    { declare(MincHandleType); idcount = 0; }
    break;

  case 12:
#line 75 "minc.y"
    {
								level--; MPRINT1("level %d", level);
								(yyval.trees) = go(tif((yyvsp[(3) - (4)].trees), (yyvsp[(4) - (4)].trees)));
							}
    break;

  case 13:
#line 79 "minc.y"
    {
								level--; MPRINT1("level %d", level);
								(yyval.trees) = go(tifelse((yyvsp[(3) - (6)].trees), (yyvsp[(4) - (6)].trees), (yyvsp[(6) - (6)].trees)));
							}
    break;

  case 14:
#line 83 "minc.y"
    {
								level--; MPRINT1("level %d", level);
								(yyval.trees) = go(twhile((yyvsp[(3) - (4)].trees), (yyvsp[(4) - (4)].trees)));
							}
    break;

  case 15:
#line 87 "minc.y"
    {
								level--; MPRINT1("level %d", level);
								(yyval.trees) = go(tfor((yyvsp[(4) - (10)].trees), (yyvsp[(6) - (10)].trees), (yyvsp[(8) - (10)].trees), (yyvsp[(10) - (10)].trees)));
							}
    break;

  case 16:
#line 91 "minc.y"
    { (yyval.trees) = (yyvsp[(2) - (3)].trees); }
    break;

  case 17:
#line 92 "minc.y"
    { flerror = 1; (yyval.trees) = tnoop(); }
    break;

  case 18:
#line 93 "minc.y"
    { flerror = 1; (yyval.trees) = tnoop(); }
    break;

  case 19:
#line 94 "minc.y"
    { flerror = 1; (yyval.trees) = tnoop(); }
    break;

  case 20:
#line 95 "minc.y"
    { flerror = 1; (yyval.trees) = tnoop(); }
    break;

  case 21:
#line 96 "minc.y"
    { flerror = 1; (yyval.trees) = tnoop(); }
    break;

  case 22:
#line 97 "minc.y"
    { flerror = 1; (yyval.trees) = tnoop(); }
    break;

  case 23:
#line 98 "minc.y"
    { flerror = 1; (yyval.trees) = tnoop(); }
    break;

  case 24:
#line 99 "minc.y"
    { flerror = 1; (yyval.trees) = tnoop(); }
    break;

  case 25:
#line 100 "minc.y"
    { flerror = 1; (yyval.trees) = tnoop(); }
    break;

  case 26:
#line 104 "minc.y"
    { level++; MPRINT1("level %d", level); }
    break;

  case 27:
#line 108 "minc.y"
    {
								sym = lookup((yyvsp[(1) - (3)].str));
								if (sym == NULL)	/* then autodeclare it */
									sym = install((yyvsp[(1) - (3)].str), S_GLOBAL);
								(yyval.trees) = tstore(tname(sym), (yyvsp[(3) - (3)].trees));
							}
    break;

  case 28:
#line 114 "minc.y"
    {
								sym = lookup((yyvsp[(1) - (3)].str));
								if (sym == NULL) {
									minc_die("'%s' is not declared", (yyvsp[(1) - (3)].str));
									(yyval.trees) = tnoop();
								}
								else
									(yyval.trees) = topassign(tname(sym), (yyvsp[(3) - (3)].trees), OpPlus);
							}
    break;

  case 29:
#line 123 "minc.y"
    {
								sym = lookup((yyvsp[(1) - (3)].str));
								if (sym == NULL) {
									minc_die("'%s' is not declared", (yyvsp[(1) - (3)].str));
									(yyval.trees) = tnoop();
								}
								else
									(yyval.trees) = topassign(tname(sym), (yyvsp[(3) - (3)].trees), OpMinus);
							}
    break;

  case 30:
#line 132 "minc.y"
    {
								sym = lookup((yyvsp[(1) - (3)].str));
								if (sym == NULL) {
									minc_die("'%s' is not declared", (yyvsp[(1) - (3)].str));
									(yyval.trees) = tnoop();
								}
								else
									(yyval.trees) = topassign(tname(sym), (yyvsp[(3) - (3)].trees), OpMul);
							}
    break;

  case 31:
#line 141 "minc.y"
    {
								sym = lookup((yyvsp[(1) - (3)].str));
								if (sym == NULL) {
									minc_die("'%s' is not declared", (yyvsp[(1) - (3)].str));
									(yyval.trees) = tnoop();
								}
								else
									(yyval.trees) = topassign(tname(sym), (yyvsp[(3) - (3)].trees), OpDiv);
							}
    break;

  case 32:
#line 151 "minc.y"
    { (yyval.trees) = tcall((yyvsp[(3) - (4)].trees), (yyvsp[(1) - (4)].str)); }
    break;

  case 33:
#line 154 "minc.y"
    { (yyval.trees) = tlist((yyvsp[(2) - (3)].trees)); }
    break;

  case 34:
#line 156 "minc.y"
    {
								sym = lookup((yyvsp[(1) - (4)].str));
								if (sym == NULL) {
									minc_die("'%s' is not declared", (yyvsp[(1) - (4)].str));
									(yyval.trees) = tnoop();
								}
								else
									(yyval.trees) = tsubscriptread(tname(sym), (yyvsp[(3) - (4)].trees));
							}
    break;

  case 35:
#line 165 "minc.y"
    {
								sym = lookup((yyvsp[(1) - (6)].str));
								if (sym == NULL) {
									minc_die("'%s' is not declared", (yyvsp[(1) - (6)].str));
									(yyval.trees) = tnoop();
								}
								else
									(yyval.trees) = tsubscriptwrite(tname(sym), (yyvsp[(3) - (6)].trees), (yyvsp[(6) - (6)].trees));
							}
    break;

  case 36:
#line 177 "minc.y"
    { idlist[idcount++] = (yyvsp[(1) - (1)].str); }
    break;

  case 37:
#line 178 "minc.y"
    { idlist[idcount++] = (yyvsp[(3) - (3)].str); }
    break;

  case 38:
#line 182 "minc.y"
    { (yyval.str) = strsave(yytext); }
    break;

  case 39:
#line 186 "minc.y"
    { (yyval.trees) = tlistelem(temptylistelem(), (yyvsp[(1) - (1)].trees)); }
    break;

  case 40:
#line 187 "minc.y"
    { (yyval.trees) = tlistelem((yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 41:
#line 192 "minc.y"
    { (yyval.trees) = temptylistelem(); }
    break;

  case 42:
#line 196 "minc.y"
    {
								char *s = yytext + 1;
								s[strlen(s) - 1] = '\0';
								(yyval.trees) = tstring(strsave(s));
							}
    break;

  case 43:
#line 204 "minc.y"
    { (yyval.trees) = (yyvsp[(1) - (1)].trees); }
    break;

  case 44:
#line 205 "minc.y"
    { (yyval.trees) = tnot((yyvsp[(2) - (2)].trees)); }
    break;

  case 45:
#line 206 "minc.y"
    { (yyval.trees) = tcand((yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 46:
#line 207 "minc.y"
    { (yyval.trees) = tcor((yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 47:
#line 208 "minc.y"
    { (yyval.trees) = trel(OpEqual, (yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 48:
#line 209 "minc.y"
    { (yyval.trees) = trel(OpNotEqual, (yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 49:
#line 210 "minc.y"
    { (yyval.trees) = trel(OpLess, (yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 50:
#line 211 "minc.y"
    { (yyval.trees) = trel(OpGreater, (yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 51:
#line 212 "minc.y"
    { (yyval.trees) = trel(OpLessEqual, (yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 52:
#line 213 "minc.y"
    { (yyval.trees) = trel(OpGreaterEqual, (yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 53:
#line 214 "minc.y"
    { (yyval.trees) = trel(OpEqual, tconstf(1.0), tconstf(1.0)); }
    break;

  case 54:
#line 215 "minc.y"
    { (yyval.trees) = trel(OpNotEqual, tconstf(1.0), tconstf(1.0)); }
    break;

  case 55:
#line 219 "minc.y"
    { (yyval.trees) = (yyvsp[(1) - (1)].trees); }
    break;

  case 56:
#line 220 "minc.y"
    { (yyval.trees) = top(OpPow, (yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 57:
#line 221 "minc.y"
    { (yyval.trees) = top(OpMul, (yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 58:
#line 222 "minc.y"
    { (yyval.trees) = top(OpDiv, (yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 59:
#line 223 "minc.y"
    { (yyval.trees) = top(OpPlus, (yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 60:
#line 224 "minc.y"
    { (yyval.trees) = top(OpMinus, (yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 61:
#line 225 "minc.y"
    { (yyval.trees) = top(OpMod, (yyvsp[(1) - (3)].trees), (yyvsp[(3) - (3)].trees)); }
    break;

  case 62:
#line 226 "minc.y"
    { (yyval.trees) = (yyvsp[(2) - (3)].trees); }
    break;

  case 63:
#line 227 "minc.y"
    { (yyval.trees) = (yyvsp[(1) - (1)].trees); }
    break;

  case 64:
#line 228 "minc.y"
    {
								double f = atof(yytext);
								(yyval.trees) = tconstf(f);
							}
    break;

  case 65:
#line 232 "minc.y"
    { (yyval.trees) = tconstf(1.0); }
    break;

  case 66:
#line 233 "minc.y"
    { (yyval.trees) = tconstf(0.0); }
    break;

  case 67:
#line 234 "minc.y"
    {
								/* tconstf is a dummy; makes exct_operator work */
								(yyval.trees) = top(OpNeg, (yyvsp[(2) - (2)].trees), tconstf(0.0));
							}
    break;

  case 68:
#line 238 "minc.y"
    {
								sym = lookup((yyvsp[(1) - (1)].str));
								if (sym == NULL) {
// FIXME: install id w/ value of 0, then warn??
									minc_die("'%s' is not declared", (yyvsp[(1) - (1)].str));
									(yyval.trees) = tconstf(0.0);
								}
								else
									(yyval.trees) = tname(sym);
							}
    break;


/* Line 1267 of yacc.c.  */
#line 1953 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 251 "minc.y"


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


