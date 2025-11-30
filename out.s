    .section .rodata
.LC0:
    .string "%d\n"
    .text
    .globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $1024, %rsp
    movq $0, %rax
    movq %rax, -8(%rbp)
    movq $1, %rax
    movq %rax, -16(%rbp)
L0:
    movq -16(%rbp), %rax
    pushq %rax
    movq $11, %rax
    popq %rbx
    cmpq %rax, %rbx
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je L1
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    popq %rbx
    addq %rbx, %rax
    movq %rax, -8(%rbp)
    movq -16(%rbp), %rax
    pushq %rax
    movq $1, %rax
    popq %rbx
    addq %rbx, %rax
    movq %rax, -16(%rbp)
    jmp L0
L1:
    movq -8(%rbp), %rax
    movq %rax, %rdx
    leaq .LC0(%rip), %rcx
    subq $32, %rsp
    call printf
    addq $32, %rsp
    movq $0, %rax
    leave
    ret
