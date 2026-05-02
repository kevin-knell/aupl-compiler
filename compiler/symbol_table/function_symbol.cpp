#include "function_symbol.hpp"
#include "return_statement.hpp"
#include "statement.hpp"
#include <color.hpp>
#include "class_db.hpp"
#include "bytecode_generator.hpp"

#define TAG(s, b) std::string(b ? s : "")

namespace cmp {
	FuncPtr FunctionSymbol::create(vm::MethodPair &method_pair) {
		return std::make_shared<FunctionSymbol>(Private(), method_pair);
	}

	FuncPtr FunctionSymbol::create(
			const TypePtr &return_type,
			const std::string &name,
			const std::vector<VarPtr> &parameters,
			const ScopePtr &scope,
			const bool is_constructor
			) {
		return std::make_shared<FunctionSymbol>(Private(), return_type, name, parameters, scope, is_constructor);
	}

FunctionSymbol::FunctionSymbol(Private, vm::MethodPair &method_pair)
		: return_type(PrimitiveType::TYPE_VOID), name(method_pair.name), method_pair(&method_pair), is_constructor(method_pair.is_constructor) {}

FunctionSymbol::FunctionSymbol(Private, const TypePtr& return_type,
		const std::string& name,
		const std::vector<VarPtr>& parameters,
		const ScopePtr& scope, const bool is_constructor)
				: return_type(std::move(return_type)), name(name), parameters(parameters), scope(std::move(scope)), is_constructor(is_constructor)
	{
		scope->name = head_to_string();
	}
	
std::string FunctionSymbol::head_to_string() {
	if (method_pair) {
		return method_pair->signature;
	}

	assert(return_type);

    std::string begin = TAG("public ", is_public) +
            TAG("static ", is_static) +
            TAG("const ", is_const) +
            return_type->to_string() + " " +
            name;
    
    std::string args = "(";

    for (VarPtr& v : parameters) {
        assert(v);
		assert(v->type);
		
		if (args.length() > 1) {
            args += ", ";
        }

        args += v->type->to_string();
        args += " ";
        args += v->name;
    }

    args += ")";

    return begin + args;
}

std::string FunctionSymbol::to_string() {
    auto head = head_to_string();

	if (method_pair) {
		return head;
	}

	assert(scope);

    std::string code;
	assert(scope->body.empty() || scope->body.front());

    if (is_abstract) {
        code = " = abstract";
    } else if (scope->body.size() == 1 && scope->body.front()->get_kind() == Statement::RETURN) {
        code = " = " + scope->body.front()->to_string();
    } else {
        code = " ";
		std::function<void(ScopePtr sc)> add_code_str = [&](ScopePtr sc) {
        	code += C_BRACE_L + "\n";
			for (StmtPtr& s : sc->body) {
				assert(s);
        	    code += s->to_string();
        	    code += "\n";
        	}
        	code += C_BRACE_R;

			for (auto lower : sc->lower_scopes) {
				if (auto lower_scope = lower.lock()) {
					code += "\n" + lower_scope->name + " ";
					add_code_str(lower_scope);
				} else {
					std::cerr << "scope is not valid" << std::endl;
				}
			}
		};
		add_code_str(scope);
    }

    return head + code;
}

std::string FunctionSymbol::to_cpp_string_prototype() {
	std::stringstream ss;

	if (is_static) {
		ss << "static ";
	}

	if (!is_constructor) {
		ss << return_type->to_cpp_type_str();
		ss << " ";
	}

	ss << name;

	ss << "(";

	for (auto it = parameters.begin(); it != parameters.end(); ++it) {
		if (it != parameters.begin()) {
			ss << ", ";
		}
		
		VarPtr arg = *it;
		ss << arg->type->to_cpp_type_str();
		ss << " ";
		ss << arg->name;
	}

	ss << ")";

	// TODO: const
	// TODO: override

	ss << ";";

	return ss.str();
}
std::string FunctionSymbol::to_cpp_string(std::string context) {
	if (method_pair) {
		return head_to_string();
	}

	std::stringstream ss;

	if (!is_constructor) {
		ss << return_type->to_cpp_type_str();
		ss << " ";
	}

	if (!context.empty()) {
		ss << context;
		ss << "::";
	}

	ss << name;

	ss << "(";

	for (auto it = parameters.begin(); it != parameters.end(); ++it) {
		if (it != parameters.begin()) {
			ss << ", ";
		}
		
		VarPtr arg = *it;
		ss << arg->type->to_cpp_type_str();
		ss << " ";
		ss << arg->name;
	}

	ss << ")";

	// TODO: const
	// TODO: override

	return ss.str();
}
}

#undef TAG