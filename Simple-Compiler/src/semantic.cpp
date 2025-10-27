#include "semantic.h"
#include "printing_options.h"
#include <iostream>
#include <tuple>
#include <stdexcept>

/******************************************************************
 * SymbolTable Implementation
 * Manages scopes and variable declarations.
 ******************************************************************/

SymbolTable::SymbolTable() {
    // Start with a global scope
    scopes.emplace_back();
}

void SymbolTable::enter_scope() {
    if (should_print(PRINT_SYMBOL_TABLE)) {
        std::cout << "Semantic: Entering new scope" << std::endl;
    }
    scopes.emplace_back();
}

void SymbolTable::exit_scope() {
    if (should_print(PRINT_SYMBOL_TABLE)) {
        std::cout << "Semantic: Exiting scope" << std::endl;
    }
    if (scopes.size() > 1) {
        scopes.pop_back();
    }
}

void SymbolTable::add_symbol(const Symbol& symbol) {
    if (scopes.back().count(symbol.name)) {
        throw std::runtime_error("Semantic Error: Variable '" + symbol.name + "' already declared in this scope.");
    }
    if (should_print(PRINT_SYMBOL_TABLE)) {
        std::cout << "Semantic: Adding symbol '" << symbol.name << "' of type '" << symbol.type << "'" << std::endl;
    }
    scopes.back().insert({symbol.name, symbol});
}

Symbol* SymbolTable::lookup_symbol(const std::string& name) {
    if (should_print(PRINT_SYMBOL_TABLE)) {
        std::cout << "Semantic: Looking up symbol '" << name << "'" << std::endl;
    }
    // Search from the innermost scope to the outermost
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return &found->second;
        }
    }
    return nullptr;
}

/******************************************************************
 * SemanticAnalyzer Implementation
 ******************************************************************/

SemanticAnalyzer::SemanticAnalyzer(CodeGen& cg) : codegen(cg) {}

void SemanticAnalyzer::reset() {
    symbol_table = SymbolTable();
}
void SemanticAnalyzer::analyze(StatementList* root) {
    if (!root) return;
    if (should_print(PRINT_PARSE_TREE)) {
        std::cout << "Semantic: Starting analysis phase..." << std::endl;
        print_ast(root);
    }
    try {
        analyze_node(root);
    } catch (const std::runtime_error& e) {
        // Re-throw to be caught by the main compiler driver
        throw e;
    }
    if (should_print(PRINT_PARSE_TREE)) {
        std::cout << "Semantic: Analysis complete. Starting TAC generation..." << std::endl;
    }
    generate_tac(root);
}

void SemanticAnalyzer::print_ast(Node* node, int indent) {
    if (!node) return;

    auto print_indent = [indent]() {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
    };

    print_indent();

    if (auto sl = dynamic_cast<StatementList*>(node)) {
        std::cout << "StatementList:" << std::endl;
        for (const auto& stmt : sl->statements) {
            print_ast(stmt.get(), indent + 1);
        }
    } else if (auto decl = dynamic_cast<Declaration*>(node)) {
        std::cout << "Declaration: " << decl->type << " " << decl->id;
        if (decl->expr) {
            std::cout << " =" << std::endl;
            print_ast(decl->expr.get(), indent + 1);
        } else {
            std::cout << std::endl;
        }
    } else if (auto assign = dynamic_cast<Assignment*>(node)) {
        std::cout << "Assignment: " << assign->id << " =" << std::endl;
        print_ast(assign->expr.get(), indent + 1);
    } else if (auto if_stmt = dynamic_cast<IfStatement*>(node)) {
        std::cout << "IfStatement:" << std::endl;
        print_indent();
        std::cout << "  Condition:" << std::endl;
        print_ast(if_stmt->condition.get(), indent + 2);
        print_indent();
        std::cout << "  If Body:" << std::endl;
        print_ast(if_stmt->if_body.get(), indent + 2);
        if (if_stmt->else_body) {
            print_indent();
            std::cout << "  Else Body:" << std::endl;
            print_ast(if_stmt->else_body.get(), indent + 2);
        }
    } else if (auto for_stmt = dynamic_cast<ForStatement*>(node)) {
        std::cout << "ForStatement:" << std::endl;
        print_indent();
        std::cout << "  Initialization:" << std::endl;
        print_ast(for_stmt->init.get(), indent + 2);
        print_indent();
        std::cout << "  Condition:" << std::endl;
        print_ast(for_stmt->condition.get(), indent + 2);
        print_indent();
        std::cout << "  Increment:" << std::endl;
        print_ast(for_stmt->increment.get(), indent + 2);
        print_indent();
        std::cout << "  Body:" << std::endl;
        print_ast(for_stmt->body.get(), indent + 2);
    } else if (auto block = dynamic_cast<Block*>(node)) {
        std::cout << "Block:" << std::endl;
        print_ast(block->statement_list.get(), indent + 1);
    } else if (auto inc = dynamic_cast<IncrementStatement*>(node)) {
        std::cout << "IncrementStatement: " << inc->id << inc->op << std::endl;
    } else if (auto binop = dynamic_cast<BinaryOp*>(node)) {
        std::cout << "BinaryOp: " << binop->op << std::endl;
        print_ast(binop->left.get(), indent + 1);
        print_ast(binop->right.get(), indent + 1);
    } else if (auto unop = dynamic_cast<UnaryOp*>(node)) {
        std::cout << "UnaryOp: " << unop->op << std::endl;
        print_ast(unop->expr.get(), indent + 1);
    } else if (auto id = dynamic_cast<Identifier*>(node)) {
        std::cout << "Identifier: " << id->name << std::endl;
    } else if (auto num = dynamic_cast<Number*>(node)) {
        std::cout << "Number: " << num->value << std::endl;
    }
}

// --- Phase 1: Semantic Analysis (Checking for errors) ---

void SemanticAnalyzer::analyze_node(Node* node) {
    if (!node) return;
    if (auto sl = dynamic_cast<StatementList*>(node)) analyze_statement_list(sl);
    else if (auto decl = dynamic_cast<Declaration*>(node)) analyze_declaration(decl);
    else if (auto assign = dynamic_cast<Assignment*>(node)) analyze_assignment(assign);
    else if (auto if_stmt = dynamic_cast<IfStatement*>(node)) analyze_if(if_stmt);
    else if (auto for_stmt = dynamic_cast<ForStatement*>(node)) analyze_for(for_stmt);
    else if (auto block = dynamic_cast<Block*>(node)) analyze_block(block);
    else if (auto id = dynamic_cast<Identifier*>(node)) analyze_id(id);
    else if (auto binop = dynamic_cast<BinaryOp*>(node)) { analyze_node(binop->left.get()); analyze_node(binop->right.get()); }
    else if (auto unop = dynamic_cast<UnaryOp*>(node)) { analyze_node(unop->expr.get()); }
    else if (dynamic_cast<IncrementStatement*>(node)) { /* ID checked in parser/TAC gen */ }
    else if (dynamic_cast<Number*>(node)) { /* No analysis needed */ }
}

void SemanticAnalyzer::analyze_statement_list(StatementList* node) {
    for (const auto& stmt : node->statements) {
        analyze_node(stmt.get());
    }
}

void SemanticAnalyzer::analyze_declaration(Declaration* node) {
    Symbol symbol(node->id, node->type);
    symbol_table.add_symbol(symbol);
    if (node->expr) {
        analyze_node(node->expr.get());
    }
}

void SemanticAnalyzer::analyze_assignment(Assignment* node) {
    if (!symbol_table.lookup_symbol(node->id)) {
        throw std::runtime_error("Semantic Error: Undeclared variable '" + node->id + "' used in assignment.");
    }
    analyze_node(node->expr.get());
}

void SemanticAnalyzer::analyze_id(Identifier* node) {
    if (!symbol_table.lookup_symbol(node->name)) {
        throw std::runtime_error("Semantic Error: Undeclared variable '" + node->name + "' used in expression.");
    }
}

void SemanticAnalyzer::analyze_if(IfStatement* node) {
    analyze_node(node->condition.get());
    analyze_node(node->if_body.get());
    if (node->else_body) {
        analyze_node(node->else_body.get());
    }
}

void SemanticAnalyzer::analyze_for(ForStatement* node) {
    symbol_table.enter_scope();
    analyze_node(node->init.get());
    analyze_node(node->condition.get());
    analyze_node(node->increment.get());
    analyze_node(node->body.get());
    symbol_table.exit_scope();
}

void SemanticAnalyzer::analyze_block(Block* node) {
    symbol_table.enter_scope();
    analyze_statement_list(node->statement_list.get());
    symbol_table.exit_scope();
}

// --- Phase 2: TAC Generation (with Indentation) ---

void SemanticAnalyzer::generate_tac(Node* node) {
    if (!node) return;
    if (auto sl = dynamic_cast<StatementList*>(node)) generate_tac_statement_list(sl);
    else if (auto decl = dynamic_cast<Declaration*>(node)) generate_tac_declaration(decl);
    else if (auto assign = dynamic_cast<Assignment*>(node)) generate_tac_assignment(assign);
    else if (auto if_stmt = dynamic_cast<IfStatement*>(node)) generate_tac_if(if_stmt);
    else if (auto for_stmt = dynamic_cast<ForStatement*>(node)) generate_tac_for(for_stmt);
    else if (auto block = dynamic_cast<Block*>(node)) generate_tac_block(block);
    else if (auto inc = dynamic_cast<IncrementStatement*>(node)) generate_tac_increment(inc);
}

void SemanticAnalyzer::generate_tac_statement_list(StatementList* node) {
    for (const auto& stmt : node->statements) {
        generate_tac(stmt.get());
    }
}

void SemanticAnalyzer::generate_tac_declaration(Declaration* node) {
    if (node->expr) {
        std::string expr_val = generate_tac_expression(node->expr.get());
        codegen.emit("MOV " + node->id + ", " + expr_val);
    }
}

void SemanticAnalyzer::generate_tac_assignment(Assignment* node) {
    std::string expr_val = generate_tac_expression(node->expr.get());
    codegen.emit("MOV " + node->id + ", " + expr_val);
}

void SemanticAnalyzer::generate_tac_increment(IncrementStatement* node) {
     if (node->op == "++") {
        codegen.emit("ADD " + node->id + ", " + node->id + ", 1");
    } else { // --
        codegen.emit("SUB " + node->id + ", " + node->id + ", 1");
    }
}

void SemanticAnalyzer::generate_tac_if(IfStatement* node) {
    auto [left, op, right] = generate_tac_condition(node->condition.get());
    std::string label_true = codegen.new_label();
    std::string label_end = codegen.new_label();

    if (node->else_body) {
        std::string label_false = codegen.new_label();
        codegen.emit("IF " + left + " " + op + " " + right + " GOTO " + label_true);
        codegen.emit("GOTO " + label_false);
        codegen.emit(label_true + ":");
        
        codegen.increase_indent();
        generate_tac(node->if_body.get());
        codegen.decrease_indent();

        codegen.emit("GOTO " + label_end);
        codegen.emit(label_false + ":");

        codegen.increase_indent();
        generate_tac(node->else_body.get());
        codegen.decrease_indent();
    } else {
        codegen.emit("IF " + left + " " + op + " " + right + " GOTO " + label_true);
        codegen.emit("GOTO " + label_end);
        codegen.emit(label_true + ":");

        codegen.increase_indent();
        generate_tac(node->if_body.get());
        codegen.decrease_indent();
    }
    codegen.emit(label_end + ":");
}

void SemanticAnalyzer::generate_tac_for(ForStatement* node) {
    generate_tac(node->init.get());
    
    std::string label_start = codegen.new_label();
    std::string label_body = codegen.new_label();
    std::string label_end = codegen.new_label();

    codegen.emit(label_start + ":");
    auto [left, op, right] = generate_tac_condition(node->condition.get());
    codegen.emit("IF " + left + " " + op + " " + right + " GOTO " + label_body);
    codegen.emit("GOTO " + label_end);

    codegen.emit(label_body + ":");
    
    codegen.increase_indent();
    generate_tac(node->body.get());
    generate_tac(node->increment.get());
    codegen.decrease_indent();

    codegen.emit("GOTO " + label_start);
    codegen.emit(label_end + ":");
}

void SemanticAnalyzer::generate_tac_block(Block* node) {
    generate_tac_statement_list(node->statement_list.get());
}

std::string SemanticAnalyzer::generate_tac_expression(Expression* expr) {
    if (auto num = dynamic_cast<Number*>(expr)) {
        std::string temp = codegen.new_temp();
        codegen.emit(temp + " = " + std::to_string(num->value));
        return temp;
    }
    if (auto id = dynamic_cast<Identifier*>(expr)) {
        // When using a variable, we don't need a new temporary.
        // We just use the variable's name directly in the TAC.
        return id->name;
    }
    if (auto binop = dynamic_cast<BinaryOp*>(expr)) {
        std::string left = generate_tac_expression(binop->left.get());
        std::string right = generate_tac_expression(binop->right.get());
        std::string temp = codegen.new_temp();
        codegen.emit(temp + " = " + left + " " + binop->op + " " + right);
        return temp;
    }
    if (auto unop = dynamic_cast<UnaryOp*>(expr)) {
        std::string val = generate_tac_expression(unop->expr.get());
        std::string temp = codegen.new_temp();
        codegen.emit(temp + " = " + unop->op + " " + val);
        return temp;
    }
    return ""; // Should not happen with a valid AST
}

std::tuple<std::string, std::string, std::string> SemanticAnalyzer::generate_tac_condition(BinaryOp* cond) {
    std::string left = generate_tac_expression(cond->left.get());
    std::string right = generate_tac_expression(cond->right.get());
    return {left, cond->op, right};
}