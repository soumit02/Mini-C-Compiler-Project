#include "compiler.h"
#include "printing_options.h"
#include <iostream>

int main() {
    setup_printing_options();

    Compiler compiler;
    std::string code = R"(
   // nested if else and nested for loop
    int n;
    n = 7;
    int num = 45;
    int var = 0;

    if (n > num) {
        var = var + 10;
    } else {
        if (var == 0) {
            var = 20;
        } else {
            int i = 0;
            int j;
            for (i = 0; i < n; i++) {
                for (j = 0; j < n; j++) {
                    var = var + j;
                }
            }
        }
    }
    )";

    if (should_print(PRINT_INPUT_CODE)) {
        std::cout << code << std::endl;
    }
    compiler.compile(code);

    return 0;
}
