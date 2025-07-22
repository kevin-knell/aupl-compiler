#pragma once

#include "variable_symbol.hpp"
#include <vector>
#include <map>
#include <memory>
#include "forward_declarations.hpp"
#include "bytecode_generation_info.hpp"

namespace cmp {

class LabelStatement;

struct Scope {
    enum SCOPE_TYPE {
        STATIC_CLASS,
        CLASS,
        FUNCTION,
        FUNCTION_SUB
    };

    std::string name;
    const SCOPE_TYPE type;
    std::weak_ptr<Scope> upper_scope;
    std::vector<std::weak_ptr<Scope>> lower_scopes;
    std::vector<StmtPtr> body;
    size_t starting_address;
    std::map<std::string, size_t> label_addresses;
    std::map<std::string, VarPtr> variables;
    std::map<std::string, int> variable_indices;
    int size;
    int temp_count = 0;
	mutable int label_count = 0;

    static std::shared_ptr<Scope> find_scope(const std::shared_ptr<Scope>& scope, const std::string& name);

    Scope(SCOPE_TYPE type) : type(type) {};
    void generate_structure(int offset = 0);
    bool has(std::string name);
    std::string get_full_name();
    std::string structure_to_string() const;
    VarPtr get_temp(TypePtr type, ExprPtr init_val, std::string temp_name = "tmp");
	std::string get_label_name(std::string name) const;
};

using ScopePtr = std::shared_ptr<Scope>;

}