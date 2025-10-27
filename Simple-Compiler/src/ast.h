#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

// Base class for all AST nodes
struct Node {
    virtual ~Node() = default;
};

// Base class for all expression nodes
struct Expression : public Node {};

// Base class for all statement nodes
struct Statement : public Node {};

// Node for a list of statements (e.g., in a block or the whole program)
struct StatementList : public Node {
    std::vector<std::unique_ptr<Statement>> statements;
};

// Expression nodes
struct Number : public Expression {
    double value;
    Number(double val) : value(val) {}
};

struct Identifier : public Expression {
    std::string name;
    Identifier(const std::string& n) : name(n) {}
};

struct BinaryOp : public Expression {
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    BinaryOp(std::string o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
};

struct UnaryOp : public Expression {
    std::string op;
    std::unique_ptr<Expression> expr;
    UnaryOp(std::string o, std::unique_ptr<Expression> e)
        : op(o), expr(std::move(e)) {}
};


// Statement nodes
struct Declaration : public Statement {
    std::string type;
    std::string id;
    std::unique_ptr<Expression> expr; // Can be nullptr if not initialized
    Declaration(const std::string& t, const std::string& i, std::unique_ptr<Expression> e = nullptr)
        : type(t), id(i), expr(std::move(e)) {}
};

struct Assignment : public Statement {
    std::string id;
    std::unique_ptr<Expression> expr;
    Assignment(const std::string& i, std::unique_ptr<Expression> e)
        : id(i), expr(std::move(e)) {}
};

struct IncrementStatement : public Statement {
    std::string id;
    std::string op; // "++" or "--"
    IncrementStatement(const std::string& i, const std::string& o) : id(i), op(o) {}
};

struct IfStatement : public Statement {
    std::unique_ptr<BinaryOp> condition;
    std::unique_ptr<Statement> if_body;
    std::unique_ptr<Statement> else_body; // Can be nullptr
    IfStatement(std::unique_ptr<BinaryOp> c, std::unique_ptr<Statement> ib, std::unique_ptr<Statement> eb = nullptr)
        : condition(std::move(c)), if_body(std::move(ib)), else_body(std::move(eb)) {}
};

struct ForStatement : public Statement {
    std::unique_ptr<Statement> init;
    std::unique_ptr<BinaryOp> condition;
    std::unique_ptr<Statement> increment;
    std::unique_ptr<Statement> body;
    ForStatement(std::unique_ptr<Statement> i, std::unique_ptr<BinaryOp> c, std::unique_ptr<Statement> inc, std::unique_ptr<Statement> b)
        : init(std::move(i)), condition(std::move(c)), increment(std::move(inc)), body(std::move(b)) {}
};

struct Block : public Statement {
    std::unique_ptr<StatementList> statement_list;
    Block(std::unique_ptr<StatementList> sl) : statement_list(std::move(sl)) {}
};


#endif // AST_H