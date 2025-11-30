#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* 어셈블리 출력을 위한 파일 포인터 (main.c 등 외부에서 정의됨) */
extern FILE *asm_out;

/* 변수 위치를 저장할 간단한 심볼 테이블 (스택 오프셋 관리용) */
typedef struct
{
    char *name;
    int offset; // 예: -8, -16 ...
} Symbol;

#define MAX_SYMBOLS 100
static Symbol sym_table[MAX_SYMBOLS];
static int sym_count = 0;
static int current_stack_offset = 0;
static int label_seq = 0; // 라벨 번호 생성용 (L0, L1...)

/* 변수 오프셋 찾기 */
static int get_var_offset(char *name)
{
    for (int i = 0; i < sym_count; i++)
    {
        if (strcmp(sym_table[i].name, name) == 0)
        {
            return sym_table[i].offset;
        }
    }
    fprintf(stderr, "Error: Undefined variable '%s'\n", name);
    exit(1);
}

/* 변수 새로 등록 (선언 시) */
static void add_var(char *name)
{
    if (sym_count >= MAX_SYMBOLS)
    {
        fprintf(stderr, "Error: Too many variables\n");
        exit(1);
    }
    current_stack_offset -= 8; // 8바이트씩(64bit 정수) 할당
    sym_table[sym_count].name = name;
    sym_table[sym_count].offset = current_stack_offset;
    sym_count++;
}

/* 코드 생성 재귀 함수 */
void gen_expr(AST *node)
{
    if (!node)
        return;

    int offset;
    int l_start, l_end;

    switch (node->type)
    {
    case AST_INT:
        fprintf(asm_out, "    movq $%d, %%rax\n", node->ival);
        break;

    case AST_VAR:
        offset = get_var_offset(node->sval);
        fprintf(asm_out, "    movq %d(%%rbp), %%rax\n", offset);
        break;

    case AST_ADD:
    case AST_SUB:
    case AST_MUL:
    case AST_DIV:
    case AST_LT:
    case AST_GT:
        /* 이항 연산: 스택 머신 방식 사용 */
        gen_expr(node->left);                  // 1. 왼쪽 계산 -> rax
        fprintf(asm_out, "    pushq %%rax\n"); // 2. 결과 스택 저장

        gen_expr(node->right);                // 3. 오른쪽 계산 -> rax
        fprintf(asm_out, "    popq %%rbx\n"); // 4. 왼쪽 값 복원 -> rbx

        /* 이제 rbx(좌)와 rax(우)를 연산 */
        if (node->type == AST_ADD)
            fprintf(asm_out, "    addq %%rbx, %%rax\n");
        else if (node->type == AST_SUB)
        {
            fprintf(asm_out, "    subq %%rax, %%rbx\n"); // rbx - rax
            fprintf(asm_out, "    movq %%rbx, %%rax\n");
        }
        else if (node->type == AST_MUL)
            fprintf(asm_out, "    imulq %%rbx, %%rax\n");
        else if (node->type == AST_DIV)
        {
            fprintf(asm_out, "    movq %%rbx, %%rax\n"); // rax = rbx(좌)
            fprintf(asm_out, "    cqo\n");               // 부호 확장
            fprintf(asm_out, "    idivq %%rax\n");       // rax / (우)
            // 몫은 rax에 남음
        }
        else if (node->type == AST_LT)
        {
            fprintf(asm_out, "    cmpq %%rax, %%rbx\n"); // rbx(좌) vs rax(우)
            fprintf(asm_out, "    setl %%al\n");         // 작으면(less) 1
            fprintf(asm_out, "    movzbq %%al, %%rax\n");
        }
        else if (node->type == AST_GT)
        {                                                /* ✨ 추가된 > 연산 처리 */
            fprintf(asm_out, "    cmpq %%rax, %%rbx\n"); // rbx(좌) vs rax(우)
            fprintf(asm_out, "    setg %%al\n");         // 크면(greater) 1
            fprintf(asm_out, "    movzbq %%al, %%rax\n");
        }
        break;

    default:
        fprintf(stderr, "Error: Unknown expression type %d\n", node->type);
        break;
    }
}

void gen_stmt(AST *node)
{
    if (!node)
        return;

    int offset;
    int l1, l2;

    switch (node->type)
    {
    case AST_VAR_DECL:
        /* 변수 선언: 심볼 테이블에 등록하고 스택 확보 */
        add_var(node->sval);
        fprintf(asm_out, "    subq $8, %%rsp\n"); // 스택 공간 확보
        break;

    case AST_ASSIGN:
        gen_expr(node->right); // 값 계산 -> rax
        offset = get_var_offset(node->left->sval);
        fprintf(asm_out, "    movq %%rax, %d(%%rbp)\n", offset);
        break;

    case AST_PRINTF:
        gen_expr(node->left);                              // 출력할 값 -> rax
        fprintf(asm_out, "    movq %%rax, %%rsi\n");       // 두 번째 인자로 값 전달
        fprintf(asm_out, "    leaq .LC0(%%rip), %%rdi\n"); // 포맷 스트링
        fprintf(asm_out, "    movq $0, %%rax\n");          // 가변 인자 개수 0
        fprintf(asm_out, "    call printf\n");
        break;

    case AST_WHILE:
        l1 = label_seq++; // 루프 시작 라벨
        l2 = label_seq++; // 루프 종료 라벨

        fprintf(asm_out, "L%d:\n", l1);
        gen_expr(node->left); // 조건 계산
        fprintf(asm_out, "    cmpq $0, %%rax\n");
        fprintf(asm_out, "    je L%d\n", l2); // 0(거짓)이면 루프 종료

        gen_stmt(node->right);                 // 루프 바디 실행 (STMT_LIST일 수 있음)
        fprintf(asm_out, "    jmp L%d\n", l1); // 다시 시작으로
        fprintf(asm_out, "L%d:\n", l2);
        break;

    case AST_STMT_LIST:
        /* 리스트의 모든 문장을 순서대로 처리 */
        AST *cur = node;
        while (cur != NULL)
        {
            /* 리스트 노드 자체는 next로 연결되어 있고, 실제 문장은 ???
               ast.c 구조상 head->next... 가 아니라,
               ast_stmt_list가 리스트를 만드는 방식에 따라 다름.
               보통 binary tree 구조로 (left=stmt, right=next_list) 혹은
               단순 linked list 구조로 구현함.

               우리가 만든 ast.c의 ast_stmt_list는 Linked List 구조(node->next)임.
               하지만 parser.y에서 $$ = ast_stmt_list($1, $2) 호출 시
               head 리스트의 끝에 tail을 붙이는 방식임.
               따라서 여기서는 그냥 노드 자체가 문장이거나, 문장들의 리스트임.

               수정: ast.c 구현을 보면 AST_STMT_LIST 타입 노드를 따로 만들지 않고,
               기존 노드의 next 포인터만 연결함.
               즉, 여기로 들어오는 node는 첫 번째 stmt임.
             */

            /* 주의: node->type이 AST_STMT_LIST가 아닐 수 있음.
               parser.y 구조상 stmt_list는 stmt들의 연결임. */

            // 현재 노드 처리 (재귀 아님, 실제 문장 타입 처리)
            // 하지만 gen_stmt가 재귀적으로 호출되므로,
            // 여기서는 그냥 list 순회만 하면 안 되고 타입을 봐야 함.

            // 아, 구조상 gen_code 진입점에서 리스트를 처리하는 게 좋음.
            // 일단 여기서는 AST_STMT_LIST 타입이 따로 없으므로(enum엔 있지만 parser가 안 만듦),
            // main 루프에서 next를 따라가게 함.
            break;
        }
        break;

    default:
        // 단일 문장인 경우 (AST_STMT_LIST 타입이 아닌 경우)
        // 위에서 처리 안 된 타입들...은 없음.
        break;
    }
}

/* 프로그램 전체 코드 생성 진입점 */
void gen_code(AST *root)
{
    if (!root)
        return;

    /* 어셈블리 헤더 출력 */
    fprintf(asm_out, "    .section .rodata\n");
    fprintf(asm_out, ".LC0:\n");
    fprintf(asm_out, "    .string \"%%d\\n\"\n"); // 출력 포맷 "%d\n"

    fprintf(asm_out, "    .text\n");
    fprintf(asm_out, "    .globl main\n");
    fprintf(asm_out, "main:\n");

    /* 프롤로그: 스택 프레임 설정 */
    fprintf(asm_out, "    pushq %%rbp\n");
    fprintf(asm_out, "    movq %%rsp, %%rbp\n");

    /* AST 순회하며 코드 생성 */
    AST *cur = root;
    while (cur != NULL)
    {
        gen_stmt(cur);
        cur = cur->next; // 다음 문장으로 이동
    }

    /* 에필로그: 스택 정리 및 리턴 */
    fprintf(asm_out, "    leave\n");
    fprintf(asm_out, "    ret\n");
}