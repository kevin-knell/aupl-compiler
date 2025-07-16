#pragma once

#include "variable_symbol.hpp"
#include <vector>
#include <map>
#include <memory>
#include "forward_declarations.hpp"

namespace cmp {

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
	std::map<std::string, VarPtr> variables;
    std::map<std::string, int> variable_indices;
    int size;
    int temp_count = 0;

    static std::shared_ptr<Scope> find_scope(const std::shared_ptr<Scope>& scope, const std::string& name);

    Scope(SCOPE_TYPE type) : type(type) {};
    void generate_structure(int offset = 0);
    bool has(std::string name);
    std::string get_full_name();
    std::string sructure_to_string() const;
    VarPtr get_temp(TypePtr type, ExprPtr init_val, std::string temp_name = "tmp");
};

using ScopePtr = std::shared_ptr<Scope>;

}