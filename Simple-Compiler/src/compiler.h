#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include "codegen.h"
#include "semantic.h"
#include "assembly_gen.h"
#include "ast.h"

// Bison's generated parser function
int yyparse();
// Flex's function to scan from a string
struct yy_buffer_state;
yy_buffer_state* yy_scan_string(const char*);
void yy_delete_buffer(yy_buffer_state*);

// Global pointer to the AST root, which the parser will populate.
// This is a common but simple way to link the parser to the driver.
extern StatementList* ast_root;

class Compiler {
public:
    Compiler();
    void compile(const std::string& text);

private:
    CodeGen codegen;
    SemanticAnalyzer semantic_analyzer;
    AssemblyGenerator asm_gen;
    
    void tokenize(const std::string& text);
};

#endif // COMPILER_H