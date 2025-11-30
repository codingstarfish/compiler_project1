%
{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

    extern int yylex(void);
    void yyerror(const char *s);

    AST *root;
    %
}

% union
{
    int ival;
    char *sval;
    AST *node;
}

    % token<ival> NUMBER % token<sval> IDENT % token BEGIN_KW END_KW INT_KW PRINT_KW WHILE_KW DO_KW IF_KW THEN_KW ELSE_KW

    % type<node> program stmt_list stmt block expr decl

    % left '<' '>' % left '+' '-' % left '*' '/'

    % %

    program
    : BEGIN_KW stmt_list END_KW '.'
{
    root = $2;
}
{
    printf("Parsing Complete! AST Created.\n");
};

stmt_list
    : stmt
{
    $$ = $1;
}
| stmt_list stmt
{
    $$ = ast_stmt_list($1, $2);
};

stmt
    : decl |
      IDENT '=' expr ';' { $$ = ast_assign(ast_var($1), $3); }
| PRINT_KW '(' expr ')' ';' { $$ = ast_printf($3); }
| WHILE_KW expr DO_KW stmt { $$ = ast_while($2, $4); }
| block ';' { $$ = $1; };

decl
    : INT_KW IDENT ';' { $$ = ast_var_decl($2, NULL); };

block
    : BEGIN_KW stmt_list END_KW { $$ = $2; };

expr
    : NUMBER { $$ = ast_int($1); }
| IDENT { $$ = ast_var($1); }
| expr '+' expr { $$ = ast_bin(AST_ADD, $1, $3); }
| expr '-' expr { $$ = ast_bin(AST_SUB, $1, $3); }
| expr '*' expr { $$ = ast_bin(AST_MUL, $1, $3); }
| expr '/' expr { $$ = ast_bin(AST_DIV, $1, $3); }
| expr '<' expr { $$ = ast_bin(AST_LT, $1, $3); }
| expr '>' expr { $$ = ast_bin(AST_GT, $1, $3); }
| '(' expr ')' { $$ = $2; };

% %

    void yyerror(const char *s)
{
    fprintf(stderr, "Parse error: %s\n", s);
}