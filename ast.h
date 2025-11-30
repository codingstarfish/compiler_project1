#ifndef AST_H
#define AST_H

/* 노드 타입 정의 (NodeType) */
typedef enum
{
    AST_INT,      // 정수 상수 (예: 10)
    AST_VAR,      // 변수 참조 (예: x)
    AST_VAR_DECL, // 변수 선언 (예: int x;)
    AST_ASSIGN,   // 대입 연산 (예: x = 10)

    /* 산술 연산 */
    AST_ADD, // +
    AST_SUB, // -
    AST_MUL, // *
    AST_DIV, // /

    /* 비교 연산 */
    AST_LT, // < (Less Than)
    AST_GT, // > (Greater Than)

    /* 제어문 및 입출력 */
    AST_PRINTF,   // print() 함수
    AST_WHILE,    // while 반복문
    AST_STMT_LIST // 문장 리스트
} NodeType;

/* AST 노드 구조체 정의 */
typedef struct AST
{
    NodeType type; // 노드 종류

    int ival;   // 정수 값 (AST_INT 일 때 사용)
    char *sval; // 변수 이름 (AST_VAR, AST_VAR_DECL 일 때 사용)

    struct AST *left;  // 왼쪽 자식 노드 (피연산자1, 조건문 등)
    struct AST *right; // 오른쪽 자식 노드 (피연산자2, 반복 내용 등)
    struct AST *next;  // 다음 문장 연결용 (Linked List 형태)
} AST;

/* AST 생성 함수 선언 (Constructor Functions) */
AST *ast_int(int value);
AST *ast_var(char *name);
AST *ast_var_decl(char *name, AST *init);
AST *ast_assign(AST *var, AST *expr);
AST *ast_bin(NodeType type, AST *left, AST *right); /* 이항 연산 (+, -, *, /, <, >) */
AST *ast_printf(AST *expr);
AST *ast_while(AST *cond, AST *body);
AST *ast_stmt_list(AST *head, AST *tail);

/* 메모리 해제 함수 */
void ast_free(AST *node);

#endif