/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SRC_PARSER_TAB_HPP_INCLUDED
# define YY_YY_SRC_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 258,                      /* ID  */
    CONSTANT = 259,                /* CONSTANT  */
    FLOAT_CONSTANT = 260,          /* FLOAT_CONSTANT  */
    STRING = 261,                  /* STRING  */
    AUTO = 262,                    /* AUTO  */
    BREAK = 263,                   /* BREAK  */
    CASE = 264,                    /* CASE  */
    CHAR = 265,                    /* CHAR  */
    CONST = 266,                   /* CONST  */
    CONTINUE = 267,                /* CONTINUE  */
    DEFAULT = 268,                 /* DEFAULT  */
    DO = 269,                      /* DO  */
    DOUBLE = 270,                  /* DOUBLE  */
    ELSE = 271,                    /* ELSE  */
    ENUM = 272,                    /* ENUM  */
    EXTERN = 273,                  /* EXTERN  */
    FLOAT = 274,                   /* FLOAT  */
    FOR = 275,                     /* FOR  */
    GOTO = 276,                    /* GOTO  */
    IF = 277,                      /* IF  */
    INT = 278,                     /* INT  */
    LONG = 279,                    /* LONG  */
    REGISTER = 280,                /* REGISTER  */
    RETURN = 281,                  /* RETURN  */
    SHORT = 282,                   /* SHORT  */
    SIGNED = 283,                  /* SIGNED  */
    SIZEOF = 284,                  /* SIZEOF  */
    STATIC = 285,                  /* STATIC  */
    STRUCT = 286,                  /* STRUCT  */
    SWITCH = 287,                  /* SWITCH  */
    TYPEDEF = 288,                 /* TYPEDEF  */
    UNION = 289,                   /* UNION  */
    UNSIGNED = 290,                /* UNSIGNED  */
    VOID = 291,                    /* VOID  */
    VOLATILE = 292,                /* VOLATILE  */
    WHILE = 293,                   /* WHILE  */
    PLUS = 294,                    /* PLUS  */
    MINUS = 295,                   /* MINUS  */
    TIMES = 296,                   /* TIMES  */
    DIVIDE = 297,                  /* DIVIDE  */
    MODULO = 298,                  /* MODULO  */
    ASSIGN = 299,                  /* ASSIGN  */
    LT = 300,                      /* LT  */
    LE = 301,                      /* LE  */
    GT = 302,                      /* GT  */
    GE = 303,                      /* GE  */
    EQ = 304,                      /* EQ  */
    NE = 305,                      /* NE  */
    SEMICOLON = 306,               /* SEMICOLON  */
    COMMA = 307,                   /* COMMA  */
    LPAREN = 308,                  /* LPAREN  */
    RPAREN = 309,                  /* RPAREN  */
    LBRACE = 310,                  /* LBRACE  */
    RBRACE = 311,                  /* RBRACE  */
    LBRACKET = 312,                /* LBRACKET  */
    RBRACKET = 313,                /* RBRACKET  */
    INCREMENT = 314,               /* INCREMENT  */
    DECREMENT = 315,               /* DECREMENT  */
    UMINUS = 316                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 23 "src/parser.y"

    std::string*   str;
    StatementList* statement_list;
    Statement*     statement;
    Expression*    expression;
    BinaryOp*      condition;

#line 133 "src/parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SRC_PARSER_TAB_HPP_INCLUDED  */
