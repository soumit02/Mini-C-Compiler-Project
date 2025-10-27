#include "codegen.h"

// MODIFIED: Initialize new members. Using "\t" for a tab character.
CodeGen::CodeGen() : temp_count(0), label_count(0), indentation_level(0), indent_char("\t") {}

std::string CodeGen::new_temp() {
    return "t" + std::to_string(++temp_count);
}

std::string CodeGen::new_label() {
    return "L" + std::to_string(++label_count);
}

// ADDED: Implementation for increasing indentation
void CodeGen::increase_indent() {
    indentation_level++;
}

// ADDED: Implementation for decreasing indentation
void CodeGen::decrease_indent() {
    if (indentation_level > 0) {
        indentation_level--;
    }
}

// MODIFIED: The emit function now handles indentation
void CodeGen::emit(const std::string& instruction) {
    // Check if the instruction is a label (e.g., "L1:", "L_start:")
    if (!instruction.empty() && instruction.back() == ':') {
        // Labels are not indented, so they stand out
        code.push_back(instruction);
    } else {
        // For all other instructions, prepend the indentation
        std::string prefix;
        for (int i = 0; i < indentation_level; ++i) {
            prefix += indent_char;
        }
        code.push_back(prefix + instruction);
    }
}

std::string CodeGen::get_code() {
    std::stringstream ss;
    for (const auto& line : code) {
        ss << line << "\n";
    }
    return ss.str();
}

// MODIFIED: Reset the indentation level along with everything else
void CodeGen::reset() {
    temp_count = 0;
    label_count = 0;
    code.clear();
    indentation_level = 0;
}