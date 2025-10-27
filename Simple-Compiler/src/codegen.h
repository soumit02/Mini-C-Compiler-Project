#ifndef CODEGEN_H
#define CODEGEN_H

#include <string>
#include <vector>
#include <sstream>

class CodeGen {
public:
    CodeGen();
    std::string new_temp();
    std::string new_label();
    void emit(const std::string& instruction);
    std::string get_code();
    void reset();

    // ADDED: Public methods to control indentation from the semantic analyzer
    void increase_indent();
    void decrease_indent();

private:
    int temp_count;
    int label_count;
    std::vector<std::string> code;

    // ADDED: Private members to manage indentation state
    int indentation_level;
    const std::string indent_char; // Use tabs or spaces for indentation
};

#endif // CODEGEN_H