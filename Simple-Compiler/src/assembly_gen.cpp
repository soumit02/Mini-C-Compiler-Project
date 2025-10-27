#include "assembly_gen.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// Helper to trim strings
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

AssemblyGenerator::AssemblyGenerator() {
    available_registers = {"eax", "ebx", "ecx", "edx", "esi", "edi"};
    register_pool = available_registers;
    spill_index = 0;
}

bool AssemblyGenerator::is_temporary(const std::string& name) {
    return name.rfind('t', 0) == 0 && isdigit(name[1]);
}

std::string AssemblyGenerator::get_register(const std::string& temp) {
    if (register_map.count(temp)) {
        return register_map[temp];
    }
    if (!register_pool.empty()) {
        std::string reg = register_pool.front();
        register_pool.erase(register_pool.begin());
        register_map[temp] = reg;
       // std::cout << "Assembly: Allocating register " << reg << " for " << temp << std::endl;
        return reg;
    }
    // Spill
    std::string reg_to_reuse = available_registers[spill_index];
    spill_index = (spill_index + 1) % available_registers.size();
    for (auto const& [key, val] : register_map) {
        if (val == reg_to_reuse) {
            register_map.erase(key);
            std::cout << "Assembly: Spilling " << key << " from " << val << " for " << temp << std::endl;
            break;
        }
    }
    register_map[temp] = reg_to_reuse;
    std::cout << "Assembly: Reusing register " << reg_to_reuse << " for " << temp << std::endl;
    return reg_to_reuse;
}

void AssemblyGenerator::free_register(const std::string& temp) {
    if (register_map.count(temp)) {
        std::string reg = register_map[temp];
        //std::cout << "Assembly: Freeing register " << reg << " from " << temp << std::endl;
        register_pool.push_back(reg);
        register_map.erase(temp);
    }
}

void AssemblyGenerator::emit(const std::string& instruction) {
    assembly_code.push_back("    " + instruction);
}

void AssemblyGenerator::emit_label(const std::string& label) {
    assembly_code.push_back(label + ":");
}

void AssemblyGenerator::add_variable(const std::string& var) {
    if (variables.find(var) == variables.end()) {
        variables.insert(var);
        data_section.push_back(var + " dd 0");
    }
}


std::string AssemblyGenerator::generate_from_tac(const std::string& tac_code) {
   // std::cout << "\n" << std::string(50, '=') << std::endl;
    //std::cout << "ASSEMBLY CODE GENERATION" << std::endl;
    //std::cout << std::string(50, '=') << std::endl;

    std::stringstream ss(tac_code);
    std::string line;

    while (std::getline(ss, line)) {
        line = trim(line);
        if (line.empty()) continue;
        //std::cout << "Processing TAC: " << line << std::endl;

        if (line.find("IF") == 0) handle_if(line);
        else if (line.find("GOTO") == 0) handle_goto(line);
        else if (line.find("ADD") == 0) handle_add_sub(line, "add");
        else if (line.find("SUB") == 0) handle_add_sub(line, "sub");
        else if (line.find("MOV") == 0) handle_mov(line);
        else if (line.find('=') != std::string::npos) handle_assignment(line);
        else if (line.back() == ':') emit_label(line.substr(0, line.length() - 1));
    }

    return get_assembly_code();
}

void AssemblyGenerator::handle_mov(const std::string& line) {
    std::string content = trim(line.substr(3));
    size_t comma_pos = content.find(',');
    std::string dest = trim(content.substr(0, comma_pos));
    std::string src = trim(content.substr(comma_pos + 1));

    add_variable(dest);
    if (is_temporary(src)) {
        std::string src_reg = get_register(src);
        emit("mov [" + dest + "], " + src_reg);
        free_register(src);
    } else { // Assuming src is a number for simplicity
        emit("mov dword [" + dest + "], " + src);
    }
}

void AssemblyGenerator::handle_add_sub(const std::string& line, const std::string& op) {
    std::string content = trim(line.substr(3));
    std::stringstream ss(content);
    std::string dest, src1, src2, token;
    std::getline(ss, dest, ',');
    std::getline(ss, src1, ',');
    std::getline(ss, src2, ',');
    dest = trim(dest);
    src1 = trim(src1);
    src2 = trim(src2);

    add_variable(dest);
    if(dest == src1){
        if(isdigit(src2[0])){
            emit(op + " dword [" + dest + "], " + src2);
        } else {
             add_variable(src2);
             emit("mov eax, [" + src2 + "]");
             emit(op + " dword [" + dest + "], eax");
        }
    }
}


void AssemblyGenerator::handle_assignment(const std::string& line) {
    size_t eq_pos = line.find('=');
    std::string dest = trim(line.substr(0, eq_pos));
    std::string expr = trim(line.substr(eq_pos + 1));

    // Simple case: t1 = 5 OR t1 = var
    if (expr.find_first_of("+-*/") == std::string::npos) {
        std::string dest_reg = get_register(dest);
        if (is_temporary(expr)) {
            emit("mov " + dest_reg + ", " + get_register(expr));
            free_register(expr);
        } else if (isdigit(expr[0]) || expr[0] == '-') {
            emit("mov " + dest_reg + ", " + expr);
        } else {
            add_variable(expr);
            emit("mov " + dest_reg + ", [" + expr + "]");
        }
    } else { // t3 = t1 + t2
        std::stringstream expr_ss(expr);
        std::string left, op, right;
        expr_ss >> left >> op >> right;

        std::string dest_reg = get_register(dest);
        if(is_temporary(left)){
            emit("mov " + dest_reg + ", " + get_register(left));
        } else if (isdigit(left[0])) {
            emit("mov " + dest_reg + ", " + left);
        } else {
            add_variable(left);
            emit("mov " + dest_reg + ", [" + left + "]");
        }

        std::string op_instr = "add";
        if(op == "-") op_instr = "sub";
        if(op == "*") op_instr = "imul";
        // Div is more complex, skipping for brevity

        if(is_temporary(right)){
             emit(op_instr + " " + dest_reg + ", " + get_register(right));
             free_register(right);
        } else if(isdigit(right[0])) {
             emit(op_instr + " " + dest_reg + ", " + right);
        } else {
            add_variable(right);
            emit(op_instr + " " + dest_reg + ", [" + right + "]");
        }
    }
}

void AssemblyGenerator::handle_if(const std::string& line) {
    std::stringstream ss(line);
    std::string token, left, op, right, go, to, label;
    ss >> token >> left >> op >> right >> go >> to >> label;

    std::string left_reg = is_temporary(left) ? get_register(left) : "eax";
    if (!is_temporary(left)) {
        add_variable(left);
        emit("mov eax, [" + left + "]");
    }

    if (is_temporary(right)) {
        emit("cmp " + left_reg + ", " + get_register(right));
    } else if (isdigit(right[0])) {
        emit("cmp " + left_reg + ", " + right);
    } else {
        add_variable(right);
        emit("cmp " + left_reg + ", [" + right + "]");
    }

    std::map<std::string, std::string> jump_map = {
        {"<", "jl"}, {">", "jg"}, {"<=", "jle"}, {">=", "jge"}, {"==", "je"}, {"!=", "jne"}
    };
    emit(jump_map[op] + " " + label);
}

void AssemblyGenerator::handle_goto(const std::string& line) {
    std::string label = trim(line.substr(4));
    emit("jmp " + label);
}

std::string AssemblyGenerator::get_assembly_code() {
    std::stringstream full_code;
    full_code << "section .data\n";
    for (const auto& var : data_section) {
        full_code << "    " << var << "\n";
    }
    full_code << "\nsection .text\n";
    full_code << "    global _start\n\n";
    full_code << "_start:\n";
    for (const auto& line : assembly_code) {
        full_code << line << "\n";
    }
    full_code << "\n    ; Exit program\n";
    full_code << "    mov eax, 1\n";
    full_code << "    xor ebx, ebx\n";
    full_code << "    int 0x80\n";
    return full_code.str();
}
