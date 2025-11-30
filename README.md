# compiler_project1
Konkuk University compiler class end of semester project
## 언어개요 및 설계의도(Language Overview)

### 언어 명칭(laneguage name):
pasCal

### 설계 의도(Design Intent):

C언어의 간결한 수식 표현과 Pascal 언어의 명확한 블록구조를 합처서 읽기 쉽고 파싱하기 쉬운 언어를 목표로 합니다.

With combination of simple mathematical expression from C language and clear block structure of Pascal Language, pasCal aims to readable and easily parsable language 

## 문법 정의(Grammar):

이 문법은 EBNF 표기법을 따릅니다.

Grammar is defined in EBNF 

### 프로그램의 전체 구조 (Structure of the program)

Program     ::= 'begin' StatementList 'end' '.'


### 문장 리스트(Statement List) 

StatementList ::= Statement
                | StatementList Statement

### 문장의 종류 (Statements)

Statement   ::= Declaration
                | Assignment
                | Loop
                | PrintStmt
                | BlockStmt ';'

### 세부 규칙 (Statement Details)

Declaration ::= 'int' IDENTIFIER ';'

Assignment  ::= IDENTIFIER '=' Expression ';'

Loop        ::= 'while' Expression 'do' Statement

PrintStmt   ::= 'print' '(' Expression ')' ';'

BlockStmt   ::= 'begin' StatementList 'end'


### 수식 (Expression)

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

## 빌드방법
## 실행방법
## 의존성