#ifndef ASSEMBLY_GEN_H
#define ASSEMBLY_GEN_H

#include <string>
#include <vector>
#include <map>
#include <set>

class AssemblyGenerator {
public:
    AssemblyGenerator();
    std::string generate_from_tac(const std::string& tac_code);

private:
    std::vector<std::string> assembly_code;
    std::vector<std::string> data_section;
    std::map<std::string, std::string> register_map;
    std::vector<std::string> available_registers;
    std::vector<std::string> register_pool;
    std::set<std::string> variables;
    int spill_index;

    bool is_temporary(const std::string& name);
    std::string get_register(const std::string& temp);
    void free_register(const std::string& temp);
    void emit(const std::string& instruction);
    void emit_label(const std::string& label);
    void add_variable(const std::string& var);
    std::string get_assembly_code();

    void handle_assignment(const std::string& line);
    void handle_mov(const std::string& line);
    void handle_if(const std::string& line);
    void handle_goto(const std::string& line);
    void handle_add_sub(const std::string& line, const std::string& op);
};

#endif // ASSEMBLY_GEN_H