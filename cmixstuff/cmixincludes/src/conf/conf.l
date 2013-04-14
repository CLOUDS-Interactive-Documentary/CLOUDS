/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
%START INIT IGNORE
%{
#include "yystype.h"
#include "conf.tab.hpp"
#define yyterminate() yy_delete_buffer(YY_CURRENT_BUFFER); return YY_NULL
#define YY_DECL int conflex(YYSTYPE *lvalp)
extern int conferror(char *);
%}

let   [a-zA-Z]
dig   [1234567890]
real  ({dig}+)((".")({dig}*)(("E"|"e")(("+"|"-")?)({dig}+))?)?
real2 (".")({dig}+)(("E"|"e")(("+"|"-")?)({dig}+))?
num   {dig}+

%%
<INIT>#                 { BEGIN IGNORE; }
<IGNORE>\n              { BEGIN INIT; }
<IGNORE>.               ;
<INIT>{real}            return TOK_NUM;
<INIT>{real2}           return TOK_NUM;
<INIT>"\""[^\"]*"\""    return TOK_STRING;
<INIT>"\'"[^\']*"\'"    return TOK_STRING;
<INIT>true              return TOK_TRUE;
<INIT>TRUE              return TOK_TRUE;
<INIT>false             return TOK_FALSE;
<INIT>FALSE             return TOK_FALSE;
<INIT>yes               return TOK_TRUE;
<INIT>YES               return TOK_TRUE;
<INIT>no                return TOK_FALSE;
<INIT>NO                return TOK_FALSE;
<INIT>on                return TOK_TRUE;
<INIT>ON                return TOK_TRUE;
<INIT>off               return TOK_FALSE;
<INIT>OFF               return TOK_FALSE;
<INIT>[=+\-*/^;]        return (*conftext);
<INIT>"\n"              ;
<INIT>" "|"\t"          ;
<INIT>{let}({let}|{dig}|"_"|".")*   return TOK_KEY;
<INIT>.                 conferror((char *) "invalid character");
.|"\n"                  { BEGIN INIT; unput(conftext[0]); }
%%
