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
"if"        { return IF_KW; }   
"else"      { return ELSE_KW; }   
"and"       { return AND_OP; }  
"or"        { return OR_OP; }  
"not"       { return NOT_OP; }    

[0-9]+      { yylval.ival = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = strdup(yytext); return IDENT; }

"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"="         { return '='; }

"=="        { return EQ_OP; }    
"!="        { return NE_OP; }    
"<="        { return LE_OP; }  
">="        { return GE_OP; }   
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