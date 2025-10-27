#ifndef PRINTING_OPTIONS_H
#define PRINTING_OPTIONS_H

#include <vector>
#include <string>

enum PrintOptions {
    PRINT_TOKENS,
    PRINT_PARSE_TREE,
    PRINT_SYMBOL_TABLE,
    PRINT_3AC,
    PRINT_ASSEMBLY,
    PRINT_INPUT_CODE,
};

void setup_printing_options();
bool should_print(PrintOptions option);

#endif // PRINTING_OPTIONS_H
