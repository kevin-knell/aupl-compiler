#include "symbol_builder.hpp"
#include <stdexcept>
#include <iostream>
#include "return_statement.hpp"
#include "class_type.hpp"
#include "static_class_type.hpp"
#include "primitive_type.hpp"
#include <cassert>

namespace cmp {

SymbolBuilder::SymbolBuilder(const std::vector<Token>& tokens, SymbolTable& symbol_table)
    : tokens(tokens), symbol_table(symbol_table), index(0) {}

bool SymbolBuilder::has_more_tokens() const {
    return index < tokens.size();
}

const Token& SymbolBuilder::peek() const {
    if (index >= tokens.size()) throw std::out_of_range("Token index out of range");
    return tokens[index];
}

const Token& SymbolBuilder::next() {
    if (index >= tokens.size()) throw std::out_of_range("Unexpected end of tokens");
    return tokens[index++];
}

bool SymbolBuilder::match(TokenType type) {
    return index < tokens.size() && tokens[index].type == type;
}

bool SymbolBuilder::expect(const std::string& value) {
    return index < tokens.size() && tokens[index].value == value;
}

void SymbolBuilder::parse_class() {
    // ==============================================================
    // parse header
    // ==============================================================

    if (!expect("class")) {
        throw std::runtime_error("Expected 'class'");
    }
    next(); // consume class

    if (!match(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected class name identifier");
    }

    std::string class_name = next().value;

    std::string parent_name;
    std::shared_ptr<ClassSymbol> parent = nullptr;

    if (expect(":")) {
        next(); // consume :

        if (!match(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected parent class identifier after 'extends'");
        }

        parent_name = next().value;

        if (symbol_table.classes.find(parent_name) == symbol_table.classes.end()) {
            parent = std::make_shared<ClassSymbol>(parent_name);
            symbol_table.classes[parent_name] = parent;
            parent->is_declared = false;
            TypePtr static_type = std::make_shared<StaticClassType>(StaticClassType(parent_name));
            VarPtr static_var = std::make_shared<VariableSymbol>(static_type, "(static)" + parent_name, nullptr);
            parent->static_var = static_var;
            parent->type = std::make_shared<ClassType>(ClassType(parent_name));
        } else {
            parent = symbol_table.classes[parent_name];
        }
    }

    auto it = symbol_table.classes.find(class_name);

    ClassPtr this_class;

    if (it == symbol_table.classes.end()) {
        // New class
        this_class = std::make_shared<ClassSymbol>(class_name);
        symbol_table.classes[class_name] = this_class;
        this_class->is_declared = true;
        TypePtr static_type = std::make_shared<StaticClassType>(StaticClassType(class_name));
        VarPtr static_var = std::make_shared<VariableSymbol>(static_type, "(static)" + class_name, nullptr);
        this_class->static_var = static_var;
        this_class->type = std::make_shared<ClassType>(ClassType(class_name));
    } else if (!it->second->is_declared) {
        // Forward-declared; complete it
        this_class = it->second;
        this_class->is_declared = true;
        this_class->parent = parent;
    } else {
        throw std::runtime_error("Class '" + class_name + "' is already declared");
    }

    this_class->static_scope = std::make_shared<Scope>(Scope::STATIC_CLASS);
    this_class->static_scope->name = "(static)" + this_class->name;

    this_class->scope = std::make_shared<Scope>(Scope::CLASS);
    this_class->scope->name = this_class->name;
    
    this_class->scope->upper_scope = this_class->static_scope;

    // ==============================================================
    // parse members / functions
    // ==============================================================
    while (has_more_tokens()) {
        if (parse_constructor(ParserInfo{this_class, nullptr, this_class->static_scope})) continue;
        if (parse_function(ParserInfo{this_class, nullptr, this_class->static_scope})) continue;
        if (parse_variable(ParserInfo{this_class, nullptr, this_class->static_scope})) continue;

        // no match
        std::cout << "no match: " << peek().value << std::endl;
        break;
    }
}

bool SymbolBuilder::parse_constructor(ParserInfo parser_info) {
    size_t start_idx = index;

    // public
    bool is_public = false;
    if (expect("public")) {
        is_public = true;
        next(); // consume public
    }

    // const
    bool is_const = false;
    if (expect("const")) {
        is_const = true;
        next(); // consume const
    }

    // constructor name (must match class name)
    if (!expect(parser_info.cls->name)) {
        index = start_idx;
        return false;
    }
    next(); // consume constructor name

    ScopePtr scope = std::make_shared<Scope>(Scope::FUNCTION);
    scope->upper_scope = parser_info.cls->scope;
    ParserInfo parser_info_header{.cls = parser_info.cls, .func = nullptr, .scope = scope};

    scope->name = "(new)";

    // args
    if (!expect("(")) {
        index = start_idx;
        return false;
    }
    next(); // consume (

    std::vector<VarPtr> parameters;

    while (!expect(")")) {
        if (!parameters.empty()) {
            if (!expect(",")) {
                next();
                continue;
            }
            next(); // consume ,
        }

        // type
        TypePtr arg_type = parse_type(parser_info_header);
        if (!arg_type) {
            std::cout << "arg type error: " << peek().value << std::endl;
            next();
            continue;
        }

        // name
        if (!match(TokenType::IDENTIFIER)) {
            std::cout << "arg name error: " << peek().value << std::endl;
            next();
            continue;
        }
        std::string arg_name = next().value;

        // initial value
        ExprPtr initial_value = nullptr;
        if (expect("=")) {
            next(); // consume =
            initial_value = parse_expression(parser_info_header);
        }

        parameters.push_back(std::make_shared<VariableSymbol>(arg_type, arg_name, initial_value));
    }
    next(); // consume )

    // body
    bool is_abstract = false;

    std::cout << "parse body" << std::endl;

    TypePtr return_type = PrimitiveType::TYPE_VOID;
    assert(return_type);
    FuncPtr constructor_symbol = std::make_shared<FunctionSymbol>(return_type, parser_info.cls->name, parameters, scope);
    constructor_symbol->is_static = true; // treat constructor as static

    ParserInfo parser_info_body{parser_info.cls, constructor_symbol, scope};

    if (expect("=")) {
        next(); // consume =

        if (expect("abstract")) {
            is_abstract = true;
            next(); // consume abstract
        } else {
            std::cout << "invalid constructor body" << std::endl;
        }
    } else if (expect("{")) {
        next(); // consume {

        while (!expect("}")) {
            std::shared_ptr<Statement> statement = parse_statement(parser_info_body);

            if (!statement) {
                std::cout << "no statement in constructor: " << peek().value << std::endl;
                next();
            } else {
                constructor_symbol->scope->body.push_back(statement);
            }
        }
        next(); // consume }
    } else {
        std::cout << "constructor has no code" << std::endl;
    }

    std::cout << "finish statements" << std::endl;

    constructor_symbol->is_public = is_public;
    constructor_symbol->is_static = false;
    constructor_symbol->is_const = is_const;
    constructor_symbol->is_abstract = is_abstract;

    std::cout << constructor_symbol->to_string() << std::endl;
    parser_info.cls->functions.insert(std::pair("(constructor)", constructor_symbol));

    return true;
}

bool SymbolBuilder::parse_function(ParserInfo parser_info) {
    size_t start_idx = index;

    // public
    bool is_public = false;
    if (expect("public")) {
        is_public = true;
        next(); // consume static
    }

    // static
    bool is_static = false;
    if (expect("static")) {
        is_static = true;
        next(); // consume static
    }

    // const
    bool is_const = false;
    if (expect("const")) {
        is_const = true;
        next(); // consume static
    }

    ScopePtr scope = std::make_shared<Scope>(Scope::FUNCTION);
    ScopePtr upper_scope = is_static ? parser_info.cls->static_scope : parser_info.cls->scope;
    scope->upper_scope = upper_scope;
    ParserInfo parser_info_header{.cls = parser_info.cls, .func = nullptr, .scope = scope};

    // return type
    TypePtr return_type = parse_type(parser_info_header);
    if (!return_type) {
        index = start_idx;
        return false;
    }

    // identifier
    if (!match(TokenType::IDENTIFIER)) {
        index = start_idx;
        return false;
    }
    std::string name = next().value;

    scope->name = name;

    // TODO f<T>(...)

    // args
    if (!expect("(")) {
        index = start_idx;
        return false;
    }
    next(); // consume (

    std::vector<VarPtr> parameters;

	if (!is_static) {
		auto this_temp = scope->get_temp(parser_info.cls->type, nullptr, "this");
		this_temp->is_const = is_const;
		parameters.push_back(this_temp);
	}

    while (!expect(")")) {
        if (!parameters.empty()) {
            if (!expect(",")) {
                next();
                continue;
            }
            next(); // consume ,
        }

        // type
        TypePtr arg_type = parse_type(parser_info_header);
        if (!arg_type) {
            // TODO: add error
            std::cout << "arg type error: " << peek().value << std::endl;
            next();
            continue;
        }

        // name
        if (!match(TokenType::IDENTIFIER)) {
            std::cout << "arg name error: " << peek().value << std::endl;
            next();
            continue;
        }
        std::string arg_name = next().value;

        // initial value
        ExprPtr initial_value = nullptr;
        if (expect("=")) {
            next(); // consume =
            initial_value = parse_expression(parser_info_header);
        }

        VarPtr param = std::make_shared<VariableSymbol>(arg_type, arg_name, initial_value);
        scope->variables[arg_name] = param;
        parameters.push_back(param);
    }
    next(); // consume )

    // body
    bool is_abstract = false;

    FuncPtr function_symbol = std::make_shared<FunctionSymbol>(return_type, name, parameters, scope);
    ParserInfo parser_info_body{parser_info.cls, function_symbol, scope};

    if (expect("=")) {
        next(); // consume =

        if (expect("abstract")) {
            is_abstract = true;
            next(); // consume abstract
        } else {
            auto expression = parse_expression(parser_info_body);
            if (expression) {
                std::shared_ptr<ReturnStatement> return_statement = std::make_shared<ReturnStatement>(expression);
                function_symbol->scope->body.push_back(return_statement);
            } else {
                std::cout << "invalid return statement" << std::endl;
            }
        }
    } else if (expect("{")) {
        next(); // consume {

        while (!expect("}")) {
			bool is_volatile = false;
			if (expect("volatile")) {
				next(); // consume volatile
				is_volatile = true;
			}

            StmtPtr statement = parse_statement(parser_info_body);
			statement->is_volatile = is_volatile;

            if (!statement) {
                std::cout << ("no statement: " + peek().value) << std::endl;
                next();
            } else {
                function_symbol->scope->body.push_back(statement);
            }
        }
        next(); // consume }
    } else {
        std::cout << "function has no code" << std::endl;
    }

    // result

    function_symbol->is_public = is_public;
    function_symbol->is_static = is_static;
    function_symbol->is_const = is_const;
    function_symbol->is_abstract = is_abstract;
    
    std::cout << function_symbol->to_string() << std::endl;
    parser_info.cls->functions.insert(std::pair(function_symbol->name, function_symbol));

    return true;
}

bool SymbolBuilder::parse_variable(ParserInfo parser_info) {
    size_t start_idx = index;

    // public
    bool is_public = false;
    if (expect("public")) {
        is_public = true;
        next(); // consume public
    }

    // static
    bool is_static = false;
    if (expect("static")) {
        is_static = true;
        next(); // consume static
    }

    // const
    bool is_const = false;
    if (expect("const")) {
        is_const = true;
        next(); // consume const
    }

    TypePtr type = parse_type(parser_info);
    if (!type) {
        index = start_idx;
        return false;
    }

    if (!match(TokenType::IDENTIFIER)) {
        index = start_idx;
        return false;
    }
    std::string name = next().value;

    ExprPtr initial_value = nullptr;
    if (expect("=")) {
        next(); // consume =
        initial_value = parse_expression(parser_info);
        if (!initial_value) {
            index = start_idx;
            return false;
        }
    }

    ScopePtr scope = is_static ? parser_info.cls->static_scope : parser_info.cls->scope;
    
    VarPtr variable_symbol = std::make_shared<VariableSymbol>(type, name, initial_value);
    variable_symbol->scope = scope;
    variable_symbol->is_public = is_public;
    variable_symbol->is_static = is_static;
    variable_symbol->is_const = is_const;

    std::cout << variable_symbol->to_string() << std::endl;
    
    scope->variables.insert(std::pair(name, variable_symbol));

    return true;
}

}