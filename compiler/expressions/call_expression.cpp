#include "call_expression.hpp"
#include "type.hpp"
#include "function_symbol.hpp"
#include "invalid_type.hpp"
#include "class_symbol.hpp"
#include "color.hpp"
#include "symbol_table.hpp"
#include "native_class_type.hpp"
#include "variable_expression.hpp"

namespace cmp {

std::vector<uint8_t> CallExpression::generate_bytecode(BytecodeGenerationInfo& bgi) const {
    (void)bgi;

	if (f->method_pair) {
		std::shared_ptr<VariableExpression> arg0 = nullptr;

		if (!arguments.empty()) {
			arg0 = std::dynamic_pointer_cast<VariableExpression>(arguments[0]);
		}

		auto method_pair = *f->method_pair;

		vm::Value2 class_id = vm::Value2::from<uint16_t>(method_pair.class_id); // Console
		vm::Value2 method_id = vm::Value2::from<uint16_t>(method_pair.method_id); // print()
		vm::Value2 obj_address = vm::Value2::from<uint16_t>(0); // static -> don't care
		vm::Value2 args_address = vm::Value2::from<uint16_t>(arg0 ? bgi.scope->variable_indices[arg0->var->name] : 0);
		vm::Value2 ret_address = vm::Value2::from<uint16_t>(0); // no ret

		return {
			(uint8_t)vm::Instruction::CALL_NATIVE,
			class_id.v[0].u8, class_id.v[1].u8,
			method_id.v[0].u8, method_id.v[1].u8,
			obj_address.v[0].u8, obj_address.v[1].u8,
			args_address.v[0].u8, args_address.v[1].u8,
			ret_address.v[0].u8, ret_address.v[1].u8,
		};
	} else {
		std::cout << to_string() << " is not native" << std::endl;
	}

    return {};
}

size_t CallExpression::get_bytecode_size(BytecodeGenerationInfo &bgi) const {
    (void)bgi;
    if (f->method_pair) {
		return 1 + 2 + 2 + 2 + 2 + 2;
	} else {
		return 0;
	}

    return 0;
}

std::string CallExpression::to_string() const {
    std::string result;

	result += obj_expr ? obj_expr->to_string() : "";
	
	result += f ? C_IDENTIFIER(f->name) : C_UNRES(name);

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
	auto native_types = name_analysis_info.symbol_table.native_types;
	auto it = native_types.find(name);
	if (it != native_types.end()) {
		std::cout << "function is type: " << it->second->to_string() << std::endl;
		auto nat = std::dynamic_pointer_cast<NativeClassType>(it->second);
		for (auto nat_func : nat->functions) {
			if (nat_func->name == name
					&& nat_func->method_pair->arg_count == arguments.size()) {
				f = nat_func;
				std::cout << "function is: " << f->to_string() << std::endl;
				return;
			}

			std::cout << nat_func->name << " " << nat_func->method_pair->arg_count << std::endl;
		}

        std::cout << "no valid constructor!" << std::endl;
		return;
	}

	auto global_native_functions = name_analysis_info.symbol_table.global_native_functions;
	std::cout << arguments.size() << std::endl;
	for (auto nat_func : global_native_functions) {
		std::cout << nat_func->to_string() << nat_func->method_pair->arg_count << std::endl;
		if (nat_func->name == name
				&& nat_func->method_pair->arg_count == arguments.size()) {
			f = nat_func;
			std::cout << "global function is: " << f->to_string() << std::endl;
			return;
		}
	}

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
    return (f && f->return_type) ? f->return_type : InvalidType::make("unknown call: " + name);
}

}