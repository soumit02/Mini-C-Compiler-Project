%{
/******************************************************************
Corrected Prologue for Bison
All #includes needed by the %union declaration MUST be here.
******************************************************************/
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "ast.h" // Defines StatementList, Statement, etc.

// Forward declare the lexer function
extern int yylex();
// The AST root pointer
extern StatementList* ast_root;
// Error reporting function
void yyerror(const char *s);
%}

/******************************************************************
Union Declaration
******************************************************************/
%union {
    std::string*   str;
    StatementList* statement_list;
    Statement*     statement;
    Expression*    expression;
    BinaryOp*      condition;
}

/* Tokens with string values */
%token <str> ID CONSTANT FLOAT_CONSTANT STRING

/* Other tokens (keywords and operators) */
%token AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO IF INT LONG REGISTER RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF UNION UNSIGNED VOID VOLATILE WHILE
%token PLUS MINUS TIMES DIVIDE MODULO ASSIGN LT LE GT GE EQ NE SEMICOLON COMMA LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET INCREMENT DECREMENT

/* Non-terminal types */
%type <statement_list> program statement_list
%type <statement> statement declaration_core assignment_core increment_core declaration assignment if_statement for_statement block increment_statement for_increment for_init
%type <expression> expression
%type <condition> condition

/* Destructors */
%destructor { delete $$; } <str>
%destructor { delete $$; } statement_list
%destructor { delete $$; } statement
%destructor { delete $$; } expression
%destructor { delete $$; } condition

/* Operator Precedence */
%left PLUS MINUS
%left TIMES DIVIDE MODULO
%right UMINUS

%%
/******************************************************************
Grammar Rules
******************************************************************/

program:
    statement_list { ast_root = $1; }
;

statement_list:
      /* empty */ { $$ = new StatementList(); }
    | statement_list statement {
        if ($2) { $1->statements.emplace_back($2); }
        $$ = $1;
    }
;

statement:
      declaration { $$ = $1; }
    | assignment { $$ = $1; }
    | if_statement { $$ = $1; }
    | for_statement { $$ = $1; }
    | block { $$ = $1; }
    | increment_statement { $$ = $1; }
    | SEMICOLON { $$ = nullptr; } /* Allow empty statements */
;

declaration_core:
      INT ID {
        $$ = new Declaration("int", *$2);
        delete $2;
      }
    | INT ID ASSIGN expression {
        $$ = new Declaration("int", *$2, std::unique_ptr<Expression>($4));
        delete $2;
      }
;

declaration:
    declaration_core SEMICOLON { $$ = $1; }
;

assignment_core:
    ID ASSIGN expression {
        $$ = new Assignment(*$1, std::unique_ptr<Expression>($3));
        delete $1;
    }
;

assignment:
    assignment_core SEMICOLON { $$ = $1; }
;

increment_core:
      ID INCREMENT {
        $$ = new IncrementStatement(*$1, "++");
        delete $1;
      }
    | ID DECREMENT {
        $$ = new IncrementStatement(*$1, "--");
        delete $1;
      }
;

increment_statement:
    increment_core SEMICOLON { $$ = $1; }
;

if_statement:
      IF LPAREN condition RPAREN statement {
        $$ = new IfStatement(std::unique_ptr<BinaryOp>($3), std::unique_ptr<Statement>($5));
      }
    | IF LPAREN condition RPAREN statement ELSE statement {
        $$ = new IfStatement(std::unique_ptr<BinaryOp>($3),
                             std::unique_ptr<Statement>($5),
                             std::unique_ptr<Statement>($7));
      }
;

for_statement:
    FOR LPAREN for_init SEMICOLON condition SEMICOLON for_increment RPAREN statement {
        $$ = new ForStatement(std::unique_ptr<Statement>($3),
                              std::unique_ptr<BinaryOp>($5),
                              std::unique_ptr<Statement>($7),
                              std::unique_ptr<Statement>($9));
    }
;

for_init:
      assignment_core { $$ = $1; }
    | declaration_core { $$ = $1; }
    | /* empty */ { $$ = nullptr; }
;

for_increment:
      assignment_core { $$ = $1; }
    | increment_core { $$ = $1; }
    | /* empty */ { $$ = nullptr; }
;

block:
    LBRACE statement_list RBRACE {
        $$ = new Block(std::unique_ptr<StatementList>($2));
    }
;

expression:
      expression PLUS expression { $$ = new BinaryOp("+", std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3)); }
    | expression MINUS expression { $$ = new BinaryOp("-", std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3)); }
    | expression TIMES expression { $$ = new BinaryOp("*", std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3)); }
    | expression DIVIDE expression { $$ = new BinaryOp("/", std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3)); }
    | MINUS expression %prec UMINUS { $$ = new UnaryOp("-", std::unique_ptr<Expression>($2)); }
    | LPAREN expression RPAREN { $$ = $2; }
    | ID { $$ = new Identifier(*$1); delete $1; }
    | CONSTANT { $$ = new Number(std::stod(*$1)); delete $1; }
;

condition:
      expression GT expression { $$ = new BinaryOp(">", std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3)); }
    | expression LT expression { $$ = new BinaryOp("<", std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3)); }
    | expression GE expression { $$ = new BinaryOp(">=", std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3)); }
    | expression LE expression { $$ = new BinaryOp("<=", std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3)); }
    | expression EQ expression { $$ = new BinaryOp("==", std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3)); }
    | expression NE expression { $$ = new BinaryOp("!=", std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3)); }
;

%%
/******************************************************************
Epilogue
******************************************************************/
#include <stdio.h>

extern char* yytext;
extern int yylineno;

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error on line %d near '%s': %s\n", yylineno, yytext, s);
}
