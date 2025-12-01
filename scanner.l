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
"if"        { return IF_KW; }     /* 추가됨 */
"else"      { return ELSE_KW; }   /* 추가됨 */
"and"       { return AND_OP; }    /* 추가됨 */
"or"        { return OR_OP; }     /* 추가됨 */
"not"       { return NOT_OP; }    /* 추가됨 */

[0-9]+      { yylval.ival = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = strdup(yytext); return IDENT; }

"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"="         { return '='; }

"=="        { return EQ_OP; }     /* == 추가 */
"!="        { return NE_OP; }     /* != 추가 */
"<="        { return LE_OP; }     /* <= 추가 */
">="        { return GE_OP; }     /* >= 추가 */
"<"         { return '<'; }
">"         { return '>'; }

"("         { return '('; }
")"         { return ')'; }
";"         { return ';'; }
"."         { return '.'; }

"//"[^\n]* { /* 한 줄 주석 처리: // 뒤의 내용은 줄바꿈 전까지 무시 */ }

[ \t\n]     { }
.           { printf("Unknown character: %c\n", *yytext); }

%%

int yywrap() { return 1; }