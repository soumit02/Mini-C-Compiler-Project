#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "codegen.h"
#include <vector>
#include <string>
#include <map>
#include <stdexcept>

class Symbol {
public:
    std::string name;
    std::string type;
    Symbol(std::string n, std::string t) : name(std::move(n)), type(std::move(t)) {}
};

class SymbolTable {
public:
    SymbolTable();
    void enter_scope();
    void exit_scope();
    void add_symbol(const Symbol& symbol);
    Symbol* lookup_symbol(const std::string& name);

private:
    std::vector<std::map<std::string, Symbol>> scopes;
};

class SemanticAnalyzer {
public:
    SemanticAnalyzer(CodeGen& codegen);
    void analyze(StatementList* root);
    void reset();

private:
    SymbolTable symbol_table;
    CodeGen& codegen;

    // Analysis methods (for checking)
    void analyze_node(Node* node);
    void analyze_statement_list(StatementList* node);
    void analyze_declaration(Declaration* node);
    void analyze_assignment(Assignment* node);
    void analyze_if(IfStatement* node);
    void analyze_for(ForStatement* node);
    void analyze_block(Block* node);
    void analyze_id(Identifier* node);
    void print_ast(Node* node, int indent = 0);
    // ... other analysis methods

    // TAC generation methods
    void generate_tac(Node* node);
    void generate_tac_statement_list(StatementList* node);
    void generate_tac_declaration(Declaration* node);
    void generate_tac_assignment(Assignment* node);
    void generate_tac_if(IfStatement* node);
    void generate_tac_for(ForStatement* node);
    void generate_tac_block(Block* node);
    void generate_tac_increment(IncrementStatement* node);

    std::string generate_tac_expression(Expression* expr);
    std::tuple<std::string, std::string, std::string> generate_tac_condition(BinaryOp* cond);
};

#endif // SEMANTIC_H