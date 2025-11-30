#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

/* 외부에서 정의된 변수 및 함수들 */
extern FILE *yyin;    // Scanner가 읽을 입력 파일 스트림
extern int yyparse(); // Parser의 진입 함수
extern AST *root;     // AST의 루트 노드 (parser.y에서 설정됨)

/* CodeGen에서 사용할 출력 파일 포인터 (codegen.c가 extern으로 참조함) */
FILE *asm_out;

/* CodeGen 함수 선언 */
void gen_code(AST *root);

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    /* 1. 입력 파일 열기 */
    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        perror("Failed to open input file");
        return 1;
    }

    /* 2. 파싱 시작 (AST 생성) */
    printf("Compiling %s...\n", argv[1]);
    if (yyparse() != 0)
    {
        fprintf(stderr, "Parsing failed due to syntax errors.\n");
        fclose(yyin);
        return 1;
    }
    printf("Parsing Complete! AST generated.\n");

    /* 3. 어셈블리 출력 파일 열기 (out.s) */
    asm_out = fopen("out.s", "w");
    if (!asm_out)
    {
        perror("Failed to open output file 'out.s'");
        fclose(yyin);
        return 1;
    }

    /* 4. 코드 생성 (AST -> x86 Assembly) */
    if (root != NULL)
    {
        gen_code(root);
        printf("Code Generation Complete! Check 'out.s'\n");
    }
    else
    {
        printf("Warning: AST root is NULL (Empty program?)\n");
    }

    /* 5. 뒷정리 */
    fclose(yyin);
    fclose(asm_out);

    // 필요하다면 메모리 해제: ast_free(root);

    return 0;
}