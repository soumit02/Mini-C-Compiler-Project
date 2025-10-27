%{
#include "ast.h"
#include "parser.tab.hpp"
#include "printing_options.h"
#include <iostream>
#include <string>          // ADDED: For std::string usage in actions

void print_token(const char* type, const char* value) {
    if (should_print(PRINT_TOKENS)) {
        std::cout << type << ": " << value << std::endl;
    }
}
%}

%option noyywrap

%%

"auto"      { print_token("Keyword", yytext); return AUTO; }
"break"     { print_token("Keyword", yytext); return BREAK; }
"case"      { print_token("Keyword", yytext); return CASE; }
"char"      { print_token("Keyword", yytext); return CHAR; }
"const"     { print_token("Keyword", yytext); return CONST; }
"continue"  { print_token("Keyword", yytext); return CONTINUE; }
"default"   { print_token("Keyword", yytext); return DEFAULT; }
"do"        { print_token("Keyword", yytext); return DO; }
"double"    { print_token("Keyword", yytext); return DOUBLE; }
"else"      { print_token("Keyword", yytext); return ELSE; }
"enum"      { print_token("Keyword", yytext); return ENUM; }
"extern"    { print_token("Keyword", yytext); return EXTERN; }
"float"     { print_token("Keyword", yytext); return FLOAT; }
"for"       { print_token("Keyword", yytext); return FOR; }
"goto"      { print_token("Keyword", yytext); return GOTO; }
"if"        { print_token("Keyword", yytext); return IF; }
"int"       { print_token("Keyword", yytext); return INT; }
"long"      { print_token("Keyword", yytext); return LONG; }
"register"  { print_token("Keyword", yytext); return REGISTER; }
"return"    { print_token("Keyword", yytext); return RETURN; }
"short"     { print_token("Keyword", yytext); return SHORT; }
"signed"    { print_token("Keyword", yytext); return SIGNED; }
"sizeof"    { print_token("Keyword", yytext); return SIZEOF; }
"static"    { print_token("Keyword", yytext); return STATIC; }
"struct"    { print_token("Keyword", yytext); return STRUCT; }
"switch"    { print_token("Keyword", yytext); return SWITCH; }
"typedef"   { print_token("Keyword", yytext); return TYPEDEF; }
"union"     { print_token("Keyword", yytext); return UNION; }
"unsigned"  { print_token("Keyword", yytext); return UNSIGNED; }
"void"      { print_token("Keyword", yytext); return VOID; }
"volatile"  { print_token("Keyword", yytext); return VOLATILE; }
"while"     { print_token("Keyword", yytext); return WHILE; }

[a-zA-Z_][a-zA-Z0-9_]* { 
    yylval.str = new std::string(yytext);
    print_token("Identifier", yytext);
    return ID; 
}

[0-9]+\.[0-9]+ { 
    yylval.str = new std::string(yytext);
    print_token("Constant", yytext);
    return FLOAT_CONSTANT; 
}
[0-9]+ { 
    yylval.str = new std::string(yytext);
    print_token("Constant", yytext);
    return CONSTANT; 
}
\"([^"\\]|\\.)*\" { 
    yylval.str = new std::string(yytext);
    print_token("String", yytext);
    return STRING; 
}

"++"        { print_token("Increment Operator", yytext); return INCREMENT; }
"--"        { print_token("Decrement Operator", yytext); return DECREMENT; }
"+"         { print_token("Arithmetic Operator", yytext); return PLUS; }
"-"         { print_token("Arithmetic Operator", yytext); return MINUS; }
"*"         { print_token("Arithmetic Operator", yytext); return TIMES; }
"/"         { print_token("Arithmetic Operator", yytext); return DIVIDE; }
"%"         { print_token("Arithmetic Operator", yytext); return MODULO; }
"="         { print_token("Relational/Assignment Operator", yytext); return ASSIGN; }
"<"         { print_token("Relational/Assignment Operator", yytext); return LT; }
">"         { print_token("Relational/Assignment Operator", yytext); return GT; }
"<="        { print_token("Relational/Assignment Operator", yytext); return LE; }
">="        { print_token("Relational/Assignment Operator", yytext); return GE; }
"=="        { print_token("Relational/Assignment Operator", yytext); return EQ; }
"!="        { print_token("Relational/Assignment Operator", yytext); return NE; }
";"         { print_token("Punctuation", yytext); return SEMICOLON; }
","         { print_token("Punctuation", yytext); return COMMA; }
"("         { print_token("Parenthesis", yytext); return LPAREN; }
")"         { print_token("Parenthesis", yytext); return RPAREN; }
"{"         { print_token("Parenthesis", yytext); return LBRACE; }
"}"         { print_token("Parenthesis", yytext); return RBRACE; }
"["         { print_token("Parenthesis", yytext); return LBRACKET; }
"]"         { print_token("Parenthesis", yytext); return RBRACKET; }

[ \t\r]+    /* ignore whitespace */
\n+         { yylineno += yyleng; }
"//".*      /* ignore single-line comments */
"/*"([^*]|\*+[^*/])*\*+"/" { yylineno += std::string(yytext).length() - std::string(yytext).find('\n'); } /* handle newlines in multi-line comments */


.           { std::cerr << "Unrecognized character: " << yytext << std::endl; }

%%