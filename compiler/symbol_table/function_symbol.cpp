#include "function_symbol.hpp"
#include "return_statement.hpp"
#include "statement.hpp"
#include <color.hpp>
#include "class_db.hpp"
#include "bytecode_generator.hpp"

#define TAG(s, b) std::string(b ? s : "")

namespace cmp {

FunctionSymbol::FunctionSymbol(vm::MethodPair &method_pair) : name(method_pair.name), method_pair(&method_pair) {}

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

}

#undef TAG