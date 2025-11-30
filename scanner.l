% {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"
      % }

    % %

    "begin"
{
    return BEGIN_KW;
}

"end" { return END_KW; }
"int" { return INT_KW; }
"print" { return PRINT_KW; }
'while' { return WHILE_KW; }
"do" {return DO_KW}

        [0 - 9] +
{
    yylval.ival = atoi(yytext);
    return NUMBER;
}

[a - zA - Z_][a - zA - Z0 - 9_] *
{
    yylval.sval = strdup(yytext);
    return IDENT;
} "+"
{
    return '+';
} /* 덧셈 연산자. ASCII 문자 그대로 반환 */
"-" { return '-'; }
"*" { return '*'; }
"/" { return '/'; }
"=" { return '='; } /* 대입 연산자 */
"<" { return '<'; } /* 비교 연산자 */
">" { return '>'; } /* 비교 연산자 */
"(" { return '('; }
")" { return ')'; }
";" { return ';'; } /* 문장 끝 세미콜론 */
"." { return '.'; } /* 프로그램 끝 마침표 (Pascal 스타일) */

[ \t\n] {}

    .
{
    printf("Unknown character: %c\n", *yytext);
} /*예외처리*/

% %

    int yywrap()
{
    return 1;
}