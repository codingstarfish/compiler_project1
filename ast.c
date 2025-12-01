#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static AST* make_node(NodeType type) {
    AST* node = (AST*)malloc(sizeof(AST));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    node->type = type;
    node->ival = 0;
    node->sval = NULL;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    node->else_body = NULL;
    return node;
}

AST* ast_int(int value) {
    AST* node = make_node(AST_INT);
    node->ival = value;
    return node;
}

AST* ast_var(char* name) {
    AST* node = make_node(AST_VAR);
    node->sval = name;
    return node;
}

AST* ast_var_decl(char* name, AST* init) {
    AST* node = make_node(AST_VAR_DECL);
    node->sval = name;
    node->right = init;
    return node;
}

AST* ast_assign(AST* var, AST* expr) {
    AST* node = make_node(AST_ASSIGN);
    node->left = var;
    node->right = expr;
    return node;
}

AST* ast_bin(NodeType type, AST* left, AST* right) {
    AST* node = make_node(type);
    node->left = left;
    node->right = right;
    return node;
}

AST* ast_unary(NodeType type, AST* left) {
    AST* node = make_node(type);
    node->left = left;
    return node;
}

AST* ast_printf(AST* expr) {
    AST* node = make_node(AST_PRINTF);
    node->left = expr;
    return node;
}

AST* ast_while(AST* cond, AST* body) {
    AST* node = make_node(AST_WHILE);
    node->left = cond;
    node->right = body;
    return node;
}

AST* ast_if(AST* cond, AST* then_body, AST* else_body) {
    AST* node = make_node(AST_IF);
    node->left = cond;
    node->right = then_body;
    node->else_body = else_body; 
    return node;
}

AST* ast_stmt_list(AST* head, AST* tail) {
    if (head == NULL) return tail;
    AST* ptr = head;
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    ptr->next = tail;
    return head;
}

void ast_free(AST* node) {
    if (node == NULL) return;
    ast_free(node->left);
    ast_free(node->right);
    ast_free(node->next);
    ast_free(node->else_body); 
    if (node->sval != NULL) free(node->sval);
    free(node);
}