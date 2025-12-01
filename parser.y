%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex(void);
void yyerror(const char* s);
extern FILE* asm_out;

AST* root;
%}

%code requires {
    #include "ast.h"
}

%union {
    int   ival;
    char* sval;
    AST* node;
}

%token <ival> NUMBER
%token <sval> IDENT
%token BEGIN_KW END_KW INT_KW PRINT_KW WHILE_KW DO_KW 
%token IF_KW ELSE_KW AND_OP OR_OP NOT_OP
%token EQ_OP NE_OP LE_OP GE_OP

%type <node> program stmt_list stmt block expr decl

/* 연산자 우선순위 (아래일수록 높음) */
%left OR_OP
%left AND_OP
%left EQ_OP NE_OP
%left '<' '>' LE_OP GE_OP
%left '+' '-'
%left '*' '/'
%right NOT_OP

%%

program
    : BEGIN_KW stmt_list END_KW '.'
      {
          root = $2;
          printf("Parsing Complete! AST Created.\n");
      }
    ;

stmt_list
    : stmt
      { $$ = $1; }
    | stmt_list stmt
      { $$ = ast_stmt_list($1, $2); }
    ;

stmt
    : decl
    | IDENT '=' expr ';'            { $$ = ast_assign(ast_var($1), $3); }
    | PRINT_KW '(' expr ')' ';'     { $$ = ast_printf($3); }
    
    /* while 조건 do 문장 */
    | WHILE_KW expr DO_KW stmt      { $$ = ast_while($2, $4); }
    
    /* if 조건 do 문장 */
    | IF_KW expr DO_KW stmt         { $$ = ast_if($2, $4, NULL); }
    
    /* if 조건 do 문장 else do 문장 (예제 코드 스타일 반영) */
    | IF_KW expr DO_KW stmt ELSE_KW DO_KW stmt { $$ = ast_if($2, $4, $7); }

    | block ';'                     { $$ = $1; }
    ;

decl
    : INT_KW IDENT ';'              { $$ = ast_var_decl($2, NULL); }
    ;

block
    : BEGIN_KW stmt_list END_KW     { $$ = $2; }
    ;

expr
    : NUMBER                        { $$ = ast_int($1); }
    | IDENT                         { $$ = ast_var($1); }
    | expr '+' expr                 { $$ = ast_bin(AST_ADD, $1, $3); }
    | expr '-' expr                 { $$ = ast_bin(AST_SUB, $1, $3); }
    | expr '*' expr                 { $$ = ast_bin(AST_MUL, $1, $3); }
    | expr '/' expr                 { $$ = ast_bin(AST_DIV, $1, $3); }
    
    /* 비교 연산 */
    | expr '<' expr                 { $$ = ast_bin(AST_LT, $1, $3); }
    | expr '>' expr                 { $$ = ast_bin(AST_GT, $1, $3); }
    | expr EQ_OP expr               { $$ = ast_bin(AST_EQ, $1, $3); }
    | expr NE_OP expr               { $$ = ast_bin(AST_NE, $1, $3); }
    | expr LE_OP expr               { $$ = ast_bin(AST_LE, $1, $3); }
    | expr GE_OP expr               { $$ = ast_bin(AST_GE, $1, $3); }
    
    /* 논리 연산 */
    | expr AND_OP expr              { $$ = ast_bin(AST_AND, $1, $3); }
    | expr OR_OP expr               { $$ = ast_bin(AST_OR, $1, $3); }
    | NOT_OP expr                   { $$ = ast_unary(AST_NOT, $2); }
    
    | '(' expr ')'                  { $$ = $2; }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n", s);
}