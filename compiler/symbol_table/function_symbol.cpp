#include "function_symbol.hpp"
#include "return_statement.hpp"
#include "statement.hpp"
#include <color.hpp>

#define TAG(s, b) std::string(b ? s : "")

namespace cmp {

size_t FunctionSymbol::get_bytecode_size(BytecodeGenerationInfo& bgi) const {
    size_t result = 0;

    for (auto stmt : scope->body) {
        BytecodeGenerationInfo bgi_stmt(bgi);
        result += stmt->get_bytecode_size(bgi_stmt);
    }

    return result;
};

std::string FunctionSymbol::head_to_string() {
    std::string begin = TAG("public ", is_public) +
            TAG("static ", is_static) +
            TAG("const ", is_const) +
            return_type->to_string() + " " +
            name;
    
    std::string args = "(";

    for (VarPtr& v : parameters) {
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

    std::string code;

    if (is_abstract) {
        code = " = abstract";
    } else if (scope->body.size() == 1 && std::dynamic_pointer_cast<ReturnStatement>(scope->body.front())) {
        code = " = " + scope->body.front()->to_string();
    } else {
        code = " " + C_BRACE_L + "\n";
        for (StmtPtr& s : scope->body) {
            code += s->to_string();
            code += "\n";
        }
        code += C_BRACE_R;
    }

    return head + code;
}

}

#undef TAG