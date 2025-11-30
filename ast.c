#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* 내부적으로 노드 메모리를 할당하는 함수 */
static AST *make_node(NodeType type)
{
    AST *node = (AST *)malloc(sizeof(AST));
    if (node == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    node->type = type;
    node->ival = 0;
    node->sval = NULL;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    return node;
}

/* 정수 노드 생성 */
AST *ast_int(int value)
{
    AST *node = make_node(AST_INT);
    node->ival = value;
    return node;
}

/* 변수 참조 노드 생성*/
AST *ast_var(char *name)
{
    AST *node = make_node(AST_VAR);
    node->sval = name; // strdup은 lexer에서 이미 처리했으므로 그대로 사용
    return node;
}

/* 변수 선언 노드 생성*/
AST *ast_var_decl(char *name, AST *init)
{
    AST *node = make_node(AST_VAR_DECL);
    node->sval = name;
    node->right = init; // 초기화 식이 있다면 right에 저장 (현재 문법에선 NULL)
    return node;
}

/* 대입 연산 노드 생성*/
AST *ast_assign(AST *var, AST *expr)
{
    AST *node = make_node(AST_ASSIGN);
    node->left = var;   // 왼쪽: 변수
    node->right = expr; // 오른쪽: 값
    return node;
}

/* 이항 연산 노드 생성*/
AST *ast_bin(NodeType type, AST *left, AST *right)
{
    AST *node = make_node(type);
    node->left = left;
    node->right = right;
    return node;
}

/* 출력문 노드 생성*/
AST *ast_printf(AST *expr)
{
    AST *node = make_node(AST_PRINTF);
    node->left = expr; // 출력할 대상
    return node;
}

/* 반복문 노드 생성*/
AST *ast_while(AST *cond, AST *body)
{
    AST *node = make_node(AST_WHILE);
    node->left = cond;  // 조건식
    node->right = body; // 실행할 문장(또는 블록)
    return node;
}

/* 문장 리스트 연결*/
AST *ast_stmt_list(AST *head, AST *tail)
{
    /* head가 NULL이면 tail만 반환 */
    if (head == NULL)
        return tail;

    /* head 리스트의 끝을 찾아서 tail을 연결 */
    AST *ptr = head;
    while (ptr->next != NULL)
    {
        ptr = ptr->next;
    }
    ptr->next = tail;

    return head;
}

/* 트리 메모리 해제 */
void ast_free(AST *node)
{
    if (node == NULL)
        return;

    /* 자식 노드들 먼저 해제 */
    ast_free(node->left);
    ast_free(node->right);
    ast_free(node->next);

    /* 문자열이 할당된 경우 해제 */
    if (node->sval != NULL)
    {
        free(node->sval);
    }

    free(node);
}