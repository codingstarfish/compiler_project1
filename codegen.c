#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern FILE* asm_out;

typedef struct {
    char* name;
    int offset;
} Symbol;

#define MAX_SYMBOLS 100
static Symbol sym_table[MAX_SYMBOLS];
static int sym_count = 0;
static int current_stack_offset = 0;
static int label_seq = 0;

static int get_var_offset(char* name) {
    for (int i = 0; i < sym_count; i++) {
        if (strcmp(sym_table[i].name, name) == 0) {
            return sym_table[i].offset;
        }
    }
    fprintf(stderr, "Error: Undefined variable '%s'\n", name);
    exit(1);
}

static void add_var(char* name) {
    if (sym_count >= MAX_SYMBOLS) {
        fprintf(stderr, "Error: Too many variables\n");
        exit(1);
    }
    current_stack_offset -= 8;
    sym_table[sym_count].name = name;
    sym_table[sym_count].offset = current_stack_offset;
    sym_count++;
}

void gen_expr(AST* node) {
    if (!node) return;
    int offset;

    switch (node->type) {
        case AST_INT:
            fprintf(asm_out, "    movq $%d, %%rax\n", node->ival);
            break;
        case AST_VAR:
            offset = get_var_offset(node->sval);
            fprintf(asm_out, "    movq %d(%%rbp), %%rax\n", offset);
            break;

        /* 단항 연산 (NOT) */
        case AST_NOT:
            gen_expr(node->left);
            fprintf(asm_out, "    cmpq $0, %%rax\n");
            fprintf(asm_out, "    sete %%al\n");
            fprintf(asm_out, "    movzbq %%al, %%rax\n");
            break;

        /* 이항 연산 */
        default:
            gen_expr(node->left);
            fprintf(asm_out, "    pushq %%rax\n");
            gen_expr(node->right);
            fprintf(asm_out, "    popq %%rbx\n"); // rbx=Left, rax=Right

            switch (node->type) {
                case AST_ADD: fprintf(asm_out, "    addq %%rbx, %%rax\n"); break; 
                case AST_SUB: 
                    fprintf(asm_out, "    subq %%rax, %%rbx\n"); 
                    fprintf(asm_out, "    movq %%rbx, %%rax\n");
                    break;
                case AST_MUL: fprintf(asm_out, "    imulq %%rbx, %%rax\n"); break;
                
                case AST_DIV:
                    fprintf(asm_out, "    movq %%rax, %%rcx\n"); 
                    fprintf(asm_out, "    movq %%rbx, %%rax\n"); 
                    fprintf(asm_out, "    cqo\n");
                    fprintf(asm_out, "    idivq %%rcx\n");
                    break;
                
                /* 비교 연산 */
                case AST_LT:
                    fprintf(asm_out, "    cmpq %%rax, %%rbx\n");
                    fprintf(asm_out, "    setl %%al\n");
                    fprintf(asm_out, "    movzbq %%al, %%rax\n");
                    break;
                case AST_GT:
                    fprintf(asm_out, "    cmpq %%rax, %%rbx\n");
                    fprintf(asm_out, "    setg %%al\n");
                    fprintf(asm_out, "    movzbq %%al, %%rax\n");
                    break;
                case AST_LE:
                    fprintf(asm_out, "    cmpq %%rax, %%rbx\n");
                    fprintf(asm_out, "    setle %%al\n");
                    fprintf(asm_out, "    movzbq %%al, %%rax\n");
                    break;
                case AST_GE:
                    fprintf(asm_out, "    cmpq %%rax, %%rbx\n");
                    fprintf(asm_out, "    setge %%al\n");
                    fprintf(asm_out, "    movzbq %%al, %%rax\n");
                    break;
                case AST_EQ:
                    fprintf(asm_out, "    cmpq %%rax, %%rbx\n");
                    fprintf(asm_out, "    sete %%al\n");
                    fprintf(asm_out, "    movzbq %%al, %%rax\n");
                    break;
                case AST_NE:
                    fprintf(asm_out, "    cmpq %%rax, %%rbx\n");
                    fprintf(asm_out, "    setne %%al\n");
                    fprintf(asm_out, "    movzbq %%al, %%rax\n");
                    break;

                /* 논리 연산 */
                case AST_AND: fprintf(asm_out, "    andq %%rbx, %%rax\n"); break;
                case AST_OR:  fprintf(asm_out, "    orq %%rbx, %%rax\n"); break;
            }
            break;
    }
}

void gen_stmt(AST* node);

void gen_seq(AST* node) {
    while (node != NULL) {
        gen_stmt(node);
        node = node->next;
    }
}

void gen_stmt(AST* node) {
    if (!node) return;
    int offset, l1, l2;

    switch (node->type) {
        case AST_VAR_DECL:
            add_var(node->sval);
            break;

        case AST_ASSIGN:
            gen_expr(node->right);
            offset = get_var_offset(node->left->sval);
            fprintf(asm_out, "    movq %%rax, %d(%%rbp)\n", offset);
            break;

        case AST_PRINTF:
            gen_expr(node->left);
#if defined(_WIN32) || defined(_WIN64)
            fprintf(asm_out, "    movq %%rax, %%rdx\n");
            fprintf(asm_out, "    leaq .LC0(%%rip), %%rcx\n");
            fprintf(asm_out, "    subq $32, %%rsp\n");
            fprintf(asm_out, "    call printf\n");
            fprintf(asm_out, "    addq $32, %%rsp\n");
#else
            fprintf(asm_out, "    movq %%rax, %%rsi\n");
            fprintf(asm_out, "    leaq .LC0(%%rip), %%rdi\n");
            fprintf(asm_out, "    movq $0, %%rax\n");
            fprintf(asm_out, "    call printf\n");
#endif
            break;

        case AST_WHILE:
            l1 = label_seq++;
            l2 = label_seq++;
            fprintf(asm_out, "L%d:\n", l1);
            gen_expr(node->left);
            fprintf(asm_out, "    cmpq $0, %%rax\n");
            fprintf(asm_out, "    je L%d\n", l2);
            gen_seq(node->right);
            fprintf(asm_out, "    jmp L%d\n", l1);
            fprintf(asm_out, "L%d:\n", l2);
            break;

        case AST_IF:
            l1 = label_seq++; // Else
            l2 = label_seq++; // End
            
            gen_expr(node->left);
            fprintf(asm_out, "    cmpq $0, %%rax\n");
            fprintf(asm_out, "    je L%d\n", l1); // 거짓이면 Else로 점프

            gen_seq(node->right); // 참일 때 실행
            fprintf(asm_out, "    jmp L%d\n", l2); // 실행 후 End로 점프

            fprintf(asm_out, "L%d:\n", l1); // Else 블록
            if (node->else_body != NULL) {  
                gen_seq(node->else_body);
            }
            
            fprintf(asm_out, "L%d:\n", l2); // 종료
            break;

        default:
            break;
    }
}

void gen_code(AST* root) {
    if (!root) return;

    fprintf(asm_out, "    .section .rodata\n");
    fprintf(asm_out, ".LC0:\n");
    fprintf(asm_out, "    .string \"%%d\\n\"\n");
    
    fprintf(asm_out, "    .text\n");
    fprintf(asm_out, "    .globl main\n");
    fprintf(asm_out, "main:\n");
    fprintf(asm_out, "    pushq %%rbp\n");
    fprintf(asm_out, "    movq %%rsp, %%rbp\n");
    fprintf(asm_out, "    subq $1024, %%rsp\n");

    gen_seq(root);

    fprintf(asm_out, "    movq $0, %%rax\n");
    fprintf(asm_out, "    leave\n");
    fprintf(asm_out, "    ret\n");
}