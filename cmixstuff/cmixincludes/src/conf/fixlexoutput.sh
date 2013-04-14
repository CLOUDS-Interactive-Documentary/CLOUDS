#!/bin/sh
#
# Replace all the global symbols in lex.conf.c with ones that don't
# conflict with our Minc parser.  The bison-generated source (conf.tab.cpp)
# uses the macros defined at the top of lex.conf.c to access these
# variables, so this substitution should not matter to it.  We use a brute
# force approach, which means that we rename some things that don't
# have to be renamed, but at least we get all the symbols that could
# possibly be a problem.                                -JGG, 7/7/04

sed \
-e 's/yy/__yy/g' \
-e 's/__yystype\.h/yystype\.h/g' \
$1 > $1.tmp

mv $1.tmp $1

