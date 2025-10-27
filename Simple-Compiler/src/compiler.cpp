#include "compiler.h"
#include "printing_options.h"
#include <iostream>
#include <stdexcept>

StatementList* ast_root = nullptr;

Compiler::Compiler() : semantic_analyzer(codegen) {}

void Compiler::tokenize(const std::string& text) {
    if (should_print(PRINT_TOKENS)) {
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "TOKENIZATION" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
    }
}

void Compiler::compile(const std::string& text) {
    codegen.reset();
    semantic_analyzer.reset();
    
    tokenize(text);

    // Set up Flex to read from the input string
    yy_buffer_state* buffer = yy_scan_string(text.c_str());
    
    if (should_print(PRINT_PARSE_TREE) || should_print(PRINT_SYMBOL_TABLE)) {
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "PARSING AND SEMANTIC ANALYSIS" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
    }

    // Run the parser. yyparse() returns 0 on success.
    if (yyparse() != 0) {
        std::cerr << "\n" << std::string(50, '=') << std::endl;
        std::cerr << "COMPILATION FAILED DUE TO SYNTAX ERROR." << std::endl;
        std::cerr << std::string(50, '=') << "\n" << std::endl;
        yy_delete_buffer(buffer);
        if (ast_root) {
            delete ast_root;
            ast_root = nullptr;
        }
        return;
    }
    
    yy_delete_buffer(buffer);

    try {
        semantic_analyzer.analyze(ast_root);
    } catch (const std::runtime_error& e) {
        std::cerr << "\n" << std::string(50, '=') << std::endl;
        std::cerr << "COMPILATION FAILED DUE TO SEMANTIC ERROR: " << e.what() << std::endl;
        std::cerr << std::string(50, '=') << "\n" << std::endl;
        if (ast_root) {
            delete ast_root;
            ast_root = nullptr;
        }
        return;
    }

    if (ast_root) {
        delete ast_root;
        ast_root = nullptr;
    }
    
    if (should_print(PRINT_3AC)) {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "GENERATED INTERMEDIATE CODE (TAC)" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        std::string tac_code = codegen.get_code();
        std::cout << tac_code << std::endl;
    }
    
    std::string tac_code = codegen.get_code();
    std::string asm_code = asm_gen.generate_from_tac(tac_code);
    
    if (should_print(PRINT_ASSEMBLY)) {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "GENERATED ASSEMBLY CODE (x86)" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        std::cout << asm_code << std::endl;
    }
}