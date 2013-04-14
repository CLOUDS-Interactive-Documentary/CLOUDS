/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

