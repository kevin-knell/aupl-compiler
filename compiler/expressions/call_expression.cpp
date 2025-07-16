#include "call_expression.hpp"
#include "type.hpp"
#include "function_symbol.hpp"
#include "invalid_type.hpp"
#include "class_symbol.hpp"
#include "color.hpp"

namespace cmp {

std::vector<uint8_t> CallExpression::generate_bytecode(BytecodeGenerationInfo& bgi) const {
    (void)bgi;
    
    std::vector<uint8_t> result;

    vm::Value2 func_size{.u16 = 0};

    result.push_back(static_cast<uint8_t>(vm::Instruction::NOP)); // todo: grow
    result.push_back(func_size.v[1].u8);
    result.push_back(func_size.v[0].u8);

    for (auto expr : arguments) {
        if (expr->get_kind() != Expression::VARIABLE) {
            std::cout << "error: expression is not variable: " << expr->to_string() << std::endl;
            continue;
        }
    }

    vm::Value4 func_address{.u32 = static_cast<uint32_t>(f->starting_address)};

    result.push_back(static_cast<uint8_t>(vm::Instruction::CALL_1));
    result.push_back(func_address.v[3].u8);
    result.push_back(func_address.v[2].u8);
    result.push_back(func_address.v[1].u8);
    result.push_back(func_address.v[0].u8);

    return result;
}

size_t CallExpression::get_bytecode_size(BytecodeGenerationInfo &bgi) const {
    (void)bgi;
    return 0;
}

std::string CallExpression::to_string() const {
    std::string result = f ? C_IDENTIFIER(f->name) : C_UNRES(name);

    result += "(";

    for (auto it = arguments.begin(); it != arguments.end(); ++it) {
        if (it != arguments.begin()) {
            result += ", ";
        }

        result += it->get()->to_string();
    }

    result += ")";

    return result;
}

bool CallExpression::is_unresolved_symbol() const {
    return f == nullptr;
}

void CallExpression::resolve(NameAnalysisInfo& name_analysis_info) {
    if (!name_analysis_info.cls) {
        std::cout << "no class" << std::endl;
        return;
    }

    for (auto [n2, f2] : name_analysis_info.cls->functions) {
        if (n2 == name) {
            f = f2;

            for (auto expr : arguments) {
                expr->resolve(name_analysis_info);
            }

            return;
        }
    }
}

std::vector<ExprPtr *> CallExpression::get_expressions() {
    std::vector<ExprPtr *> result;

    for (auto& expr : arguments) {
        result.push_back(&expr);
    }

    return result;
}

bool CallExpression::is_pure() const {
    return f->is_pure;
}

TypePtr CallExpression::get_type() const {
    return f ? f->return_type : InvalidType::make("unknown call: " + name);
}

}