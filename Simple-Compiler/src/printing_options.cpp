#include "printing_options.h"
#include <iostream>
#include <sstream>
#include <algorithm>

static std::vector<bool> print_enabled(6, false);

void setup_printing_options() {
    std::cout << "Which steps to print?" << std::endl;
    std::cout << "1. Tokens" << std::endl;
    std::cout << "2. Parse Tree" << std::endl;
    std::cout << "3. Symbol Table" << std::endl;
    std::cout << "4. 3-Address Code" << std::endl;
    std::cout << "5. Assembly" << std::endl;
    std::cout << "Enter numbers separated by spaces (e.g., 1 2 3): ";

    std::string line;
    std::getline(std::cin, line);

    if (line.empty()) {
        print_enabled[PRINT_INPUT_CODE] = true;
        print_enabled[PRINT_ASSEMBLY] = true;
        return;
    }

    std::stringstream ss(line);
    int option;
    while (ss >> option) {
        if (option >= 1 && option <= 5) {
            print_enabled[option - 1] = true;
        }
    }
    print_enabled[PRINT_INPUT_CODE] = true;
}

bool should_print(PrintOptions option) {
    return print_enabled[option];
}
