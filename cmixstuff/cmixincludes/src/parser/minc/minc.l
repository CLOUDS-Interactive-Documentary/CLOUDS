/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
%START INIT IGNORE COM CPCOM
%x INCLUDE
%{
#include "y.tab.h"
#define YY_ALWAYS_INTERACTIVE 1
static int comments = 0;
static int cpcomments = 0;
extern void yyerror(char *);
/* To allow includes of subfiles */
#define MAX_INCLUDE_DEPTH 10
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;
%}

let   [a-zA-Z]
dig   [1234567890]
real  ({dig}+)((".")({dig}*)(("E"|"e")(("+"|"-")?)({dig}+))?)?
real2 (".")({dig}+)(("E"|"e")(("+"|"-")?)({dig}+))?
num   {dig}+

%%
<INIT>"/*"     { BEGIN COM; comments++; }
<INIT>"//"     { BEGIN CPCOM; cpcomments = 1; }
<INIT>#        { BEGIN IGNORE; }
<IGNORE>\n     { BEGIN INIT; }
<IGNORE>.      ;
<COM>"/*"      { comments++; }
<COM>.|\n      ;
<COM>"*/"      { comments--; if (comments == 0 && !cpcomments) { BEGIN INIT; } }
<CPCOM>\n      { cpcomments = 0; if (comments == 0) { BEGIN INIT; } }
<CPCOM>.    ;
<INIT>{real}            return TOK_NUM;
<INIT>{real2}           return TOK_NUM;
<INIT>"<>"|"!="         return TOK_UNEQU;
<INIT>"<="              return TOK_LESSEQU;
<INIT>">="              return TOK_GTREQU;
<INIT>"=="              return TOK_EQU;
<INIT>"+="              return TOK_PLUSEQU;
<INIT>"-="              return TOK_MINUSEQU;
<INIT>"*="              return TOK_MULEQU;
<INIT>"/="              return TOK_DIVEQU;
<INIT>"**"|"^"          return TOK_POW;
<INIT>"\""[^\"]*"\""    return TOK_STRING;
<INIT>"\'"[^\']*"\'"    return TOK_STRING;
<INIT>for               return TOK_FOR;
<INIT>if                return TOK_IF;
<INIT>else              return TOK_ELSE;
<INIT>while             return TOK_WHILE;
<INIT>and               return TOK_AND;
<INIT>"&&"              return TOK_AND;
<INIT>not               return TOK_NOT;
<INIT>"!"|"~"           return TOK_NOT;
<INIT>or                return TOK_OR;
<INIT>"||"              return TOK_OR;
<INIT>true              return TOK_TRUE;
<INIT>TRUE              return TOK_TRUE;
<INIT>false             return TOK_FALSE;
<INIT>FALSE             return TOK_FALSE;
<INIT>float             return TOK_FLOAT_DECL;
<INIT>string            return TOK_STRING_DECL;
<INIT>handle            return TOK_HANDLE_DECL;
<INIT>[%=><{},;*+-]     return (*yytext);
<INIT>"["|"]"|"/"       return (*yytext);
<INIT>"("|")"           return (*yytext);
<INIT>"\n"        ;
<INIT>" "|"\t"       ;
<INIT>^include		  	{ BEGIN INCLUDE; }
<INIT>({let}|"_")({let}|{dig}|"#"|"_"|".")*   return TOK_IDENT;
<INIT>.                 yyerror("invalid character");
^include		  		{ BEGIN INCLUDE; }
.|"\n"                  { BEGIN INIT; unput(yytext[0]); }
<INCLUDE>[ \t]*      /* eat the whitespace */
<INCLUDE>[^ \t\n]+  { /* got the include file name */
			if (include_stack_ptr >= MAX_INCLUDE_DEPTH)
			{
				yyerror( "Includes nested too deeply");
				exit(1);
			}
			include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
			yyin = fopen( yytext, "r" );
			if (!yyin)
			{
				char msg[1024];
				snprintf(msg, 1024, "Unable to open include file \"%s\"",
						 yytext);
				yyerror(msg);
				exit(1);
			}
			yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
			BEGIN INIT;
		}

<<EOF>> {	if (--include_stack_ptr < 0)
			{
				yyterminate();
			}
			else {
				yy_delete_buffer(YY_CURRENT_BUFFER);
				yy_switch_to_buffer(include_stack[include_stack_ptr]);
			}
		} 
%%

