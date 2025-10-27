

# C_Compiler

A simple compiler for a C-like language.

## Features

*   **Data Types:** `int`
*   **Variables:** Declaration and assignment.
*   **Control Flow:** `if-else` statements and `for` loops.
*   **Operators:** `+`, `-`, `*`, `/`
*   **Relational Operators:** `>`, `<`, `>=`, `<=`, `==`, `!=`
*   **Other:** `++`, `--`
*   **Comments:** C-style single-line (`//`) and multi-line (`/* ... */`) comments are supported.

## How to Build and Run

To build and run the compiler, you can use the provided shell script:

```bash
./run.sh
```

This will clean the previous build, build the compiler, and run it.

Alternatively, you can build the compiler manually:

```bash
make
```

This will create the executable file in the `bin` directory.

## How to Use

The compiler currently takes a hardcoded string from `src/main.cpp`. To change the code to be compiled, you need to modify the `code` variable in `src/main.cpp`.

To run the compiler after a manual build, execute the following command:

```bash
./bin/compiler
```
