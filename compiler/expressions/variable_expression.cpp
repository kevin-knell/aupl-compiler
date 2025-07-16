#include "variable_expression.hpp"
#include "scope.hpp"
#include "instructions.hpp"
#include "symbol_table.hpp"
#include "variable_symbol.hpp"
#include "invalid_type.hpp"
#include "color.hpp"

namespace cmp {

std::vector<uint8_t> VariableExpression::generate_bytecode(BytecodeGenerationInfo& bgi) const {
    return {
        static_cast<uint8_t>(vm::Instruction::ERR)
    };
}

size_t VariableExpression::get_bytecode_size(BytecodeGenerationInfo &bgi) const {
    (void)bgi;
    return 1;
}

std::string VariableExpression::to_string() const {
    return is_unresolved_symbol() == true ? C_UNRES(name) : var->name_to_string();
}

bool VariableExpression::is_unresolved_symbol() const {
    return var == nullptr;
}

void VariableExpression::resolve(NameAnalysisInfo& name_analysis_info) {
    ScopePtr scope = Scope::find_scope(name_analysis_info.f->scope, name);
    if (scope) {
        var = scope->variables[name];
        name_analysis_info.cls = nullptr;
    } else {
        auto it = name_analysis_info.symbol_table.classes.find(name);
        if (it != name_analysis_info.symbol_table.classes.end()) {
            var = it->second->static_var;
            name_analysis_info.cls = it->second;
        }
    }
}

TypePtr VariableExpression::get_type() const {
    return var ? var->type : InvalidType::make("unknown variable: " + name);
}

bool VariableExpression::is_constexpr() const {
    if (!var->initial_value) return false;
    else return var->initial_value->is_constexpr();
}

vm::Value* VariableExpression::eval_constexpr() const {
    if (!var->initial_value) return nullptr;
    else return var->initial_value->eval_constexpr();
}

}