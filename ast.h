#ifndef AST_H
#define AST_H

typedef enum {
    AST_INT,
    AST_VAR,
    AST_VAR_DECL,
    AST_ASSIGN,

    /* 산술 */
    AST_ADD, AST_SUB, AST_MUL, AST_DIV,

    /* 비교 */
    AST_LT, AST_GT, 
    AST_EQ, // ==
    AST_NE, // !=
    AST_LE, // <=
    AST_GE, // >=

    /* 논리 */
    AST_AND, AST_OR, AST_NOT,

    /* 제어 */
    AST_PRINTF,
    AST_WHILE,
    AST_IF,          // if 문
    AST_STMT_LIST
} NodeType;

typedef struct AST {
    NodeType type;
    int ival;
    char* sval;
    struct AST* left;
    struct AST* right;
    struct AST* next;      // 다음 문장 연결용 (Linked List)
    struct AST* else_body; 
} AST;

AST* ast_int(int value);
AST* ast_var(char* name);
AST* ast_var_decl(char* name, AST* init);
AST* ast_assign(AST* var, AST* expr);
AST* ast_bin(NodeType type, AST* left, AST* right);
AST* ast_unary(NodeType type, AST* left);
AST* ast_printf(AST* expr);
AST* ast_while(AST* cond, AST* body);
AST* ast_if(AST* cond, AST* then_body, AST* else_body);
AST* ast_stmt_list(AST* head, AST* tail);
void ast_free(AST* node);

#endif