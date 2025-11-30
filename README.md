# compiler_project1
Konkuk University compiler class final project
### 언어개요 및 설계의도

##### 언어 명칭(laneguage name):
pasCal

##### 설계 의도(Design Intent):

C언어의 간결한 수식 표현과 Pascal 언어의 명확한 블록구조를 합처서 읽기 쉽고 파싱하기 쉬운 언어를 목표로 합니다.

With combination of simple mathematical expression from C language and clear block structure of Pascal Language, pasCal aims to readable and easily parsable language 

##### 문법 정의(Grammar)

이 문법은 EBNF 표기법을 따릅니다.

/* 1. 프로그램의 전체 구조 */
Program     ::= 'begin' StatementList 'end' '.'

/* 2. 문장 리스트 */
StatementList ::= Statement
                | StatementList Statement

/* 3. 문장의 종류 */
Statement   ::= Declaration
                | Assignment
                | Loop
                | PrintStmt
                | BlockStmt ';'

/* 4. 세부 규칙 */

/* 변수 선언: int x; */
Declaration ::= 'int' IDENTIFIER ';'

/* 대입 연산: x = 10; */
Assignment  ::= IDENTIFIER '=' Expression ';'

/* 반복문: while 조건 do 문장 */
Loop        ::= 'while' Expression 'do' Statement

/* 출력문: print(x); */
PrintStmt   ::= 'print' '(' Expression ')' ';'

/* 블록: begin ... end (내부 블록) */
BlockStmt   ::= 'begin' StatementList 'end'

/* 5. 수식 (Expression) - 우선순위 반영 */
Expression  ::= AddExpr
              | Expression '<' AddExpr  /* 비교 연산 */

AddExpr     ::= Term
              | AddExpr '+' Term
              | AddExpr '-' Term

Term        ::= Factor
              | Term '*' Factor
              | Term '/' Factor

Factor      ::= NUMBER
              | IDENTIFIER
              | '(' Expression ')' 