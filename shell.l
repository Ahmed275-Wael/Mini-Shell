
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h"

%}

%%

\n 	{
		return NEWLINE;
	}

[ \t] 	{
		/* Discard spaces and tabs */
     	}	

">" 	{
		return GREAT;
	}
"<"     {
         return LESS;
        }
">>"    {
         return GREATGREAT;
        }
">&"    {
         return GREATAMPERSAND;
        }
"|"     {
         return PIPE;
        }
"&"     {
         return AMPERSAND;
        }
.       {
        /*Invalidcharacterininput*/
        return  NOTOKEN;
        }

[^ \t\n][^ \t\n]*  {
		/* Assume that file names have only alpha chars */
		yylval.string_val = strdup(yytext);
		return WORD;
	}
%%
	