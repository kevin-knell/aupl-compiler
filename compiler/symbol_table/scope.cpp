#include "scope.hpp"

#include <iostream>
#include <sstream>

#include "variable_symbol.hpp"
#include <iomanip>

namespace cmp {

std::shared_ptr<Scope> Scope::find_scope(const std::shared_ptr<Scope>& scope, const std::string& name) {
    if (scope->has(name)) {
        return scope;
    } else if (auto sp = scope->upper_scope.lock()) {
        return find_scope(sp, name);
    } else {
        return nullptr;
    }
}

void Scope::generate_structure(int offset) {
    variable_indices.clear();
    for (auto& [name, var] : variables) {
        int var_size = var->type->get_size();
        if (var_size) {
            variable_indices[name] = offset;
            offset += var_size;
            size += var_size;
        }
    }

    if (type == FUNCTION || type == FUNCTION_SUB) {
        for (auto wp : lower_scopes) {
            if (auto sp = wp.lock()) {
                sp->generate_structure(offset);
            }
        }
    }
}

bool Scope::has(std::string name) {
    return variables.count(name);
}

std::string Scope::get_full_name() {
    if (auto sp = upper_scope.lock())
        return sp->get_full_name() + "." + name;
    return name;
}

std::string Scope::sructure_to_string() const {
    std::stringstream result;
    for (auto [name, offset] : variable_indices) {
        VarPtr var = variables.find(name)->second;
        int var_size = var->type->get_size();
        result << std::setw(4) << std::hex << offset << " to " << std::setw(4) << offset + var_size - 1 << ": " << var->to_string() << "\n";
    }
    return result.str();
}

VarPtr Scope::get_temp(TypePtr type, ExprPtr init_val, std::string temp_name) {
    std::string name = "%" + temp_name + std::to_string(temp_count++);
    VarPtr var = std::make_shared<VariableSymbol>(type, name, init_val);
    var->is_const = true;
    variables[name] = var;
    return var;
}

}