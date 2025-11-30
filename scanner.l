%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"
%}

%%

"begin"     { return BEGIN_KW; }
"end"       { return END_KW; }
"int"       { return INT_KW; }
"print"     { return PRINT_KW; }
"while"     { return WHILE_KW; }
"do"        { return DO_KW; }

[0-9]+      { yylval.ival = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = strdup(yytext); return IDENT; }

"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"="         { return '='; }
"<"         { return '<'; }
">"         { return '>'; }
"("         { return '('; }
")"         { return ')'; }
";"         { return ';'; }
"."         { return '.'; }

[ \t\n]     { }
.           { printf("Unknown character: %c\n", *yytext); }

%%

int yywrap() { return 1; }