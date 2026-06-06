#include "symbol_builder.hpp"
#include <stdexcept>
#include <iostream>
#include "return_statement.hpp"
#include "class_type.hpp"
#include "static_class_type.hpp"
#include "primitive_type.hpp"
#include "pointer_type.hpp"
#include "shared_type.hpp"
#include "compiler_error.hpp"
#include <format>

namespace cmp {

SymbolBuilder::SymbolBuilder(
		const SourceFile& source_file,
		SymbolTable& symbol_table)
    		:	source_file(source_file),
				tokens(source_file.tokens),
				symbol_table(symbol_table),
				index(0) {}

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

Error& SymbolBuilder::add_error(size_t start_idx, const std::string message, Error::Level level) {
	SourceLocation source_location(&source_file, start_idx, index);
	return symbol_table.add_error(source_location, message, level);
}

void SymbolBuilder::parse_class() {
    // ==============================================================
    // parse header
    // ==============================================================

	// class name
	//		'class' IDENTIFIER
    if (!expect("class")) {
		add_error(0, "Expected 'class'", Error::ERROR);
    } else {
		(void) next(); // consume 'class'
	}

    if (!match(TokenType::IDENTIFIER)) {
		add_error(0, "Expected class name identifier", Error::ERROR);
    } else {
		class_name = next().value;
	}

	// parent
	//		':' IDENTIFIER
    std::string parent_name;
    std::shared_ptr<ClassSymbol> parent;

    if (expect(":")) {
        next(); // consume ':'

        if (!match(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected parent class identifier after 'extends'");
        }

        parent_name = next().value;

        if (symbol_table.classes.find(parent_name) == symbol_table.classes.end()) {
            parent = ClassSymbol::create(parent_name);
            symbol_table.classes[parent_name] = parent;
            parent->is_declared = false;
        } else {
            parent = symbol_table.classes[parent_name];

			if (auto nat = parent->native_class_bind) {
				if (!nat->is_object) {
			        throw std::runtime_error("Parent Class must inherit from 'Object'!");
				}
			}
        }
    } else {
		parent = symbol_table.classes["Object"];
	}

    auto it = symbol_table.classes.find(class_name);

    ClassPtr this_class;

    if (it == symbol_table.classes.end()) {
        // New class
        this_class = ClassSymbol::create(class_name);
        symbol_table.classes[class_name] = this_class;
        this_class->is_declared = true;
    } else if (!it->second->is_declared) {
        // Forward-declared; complete it
        this_class = it->second;
        this_class->is_declared = true;
    } else {
		add_error(index, "Class '" + class_name + "' is already declared", Error::CRITICAL);
		return;
    }

	this_class->parent = parent;

    this_class->static_scope = Scope::create(Scope::STATIC_CLASS, "(static)" + this_class->name);

    this_class->scope = Scope::create(Scope::CLASS, this_class->name);
    this_class->scope->upper_scope = this_class->static_scope;

    // ==============================================================
    // parse members / functions
    // ==============================================================
    while (has_more_tokens()) {
		size_t error_start = index;
		
		while (has_more_tokens()
				&& !peek().has_flag(TokenFlagBits::FILE_ELEMENT_BEGIN)) {
			next();
		}

		if (error_start != index) {
			add_error(error_start, "Invalid start for header element: " + tokens[error_start].value, Error::ERROR);
			continue;
		}

        if (parse_constructor(ParserInfo(this_class, nullptr, this_class->static_scope))) continue;
        if (parse_function(ParserInfo(this_class, nullptr, this_class->static_scope))) continue;
        if (parse_operator(ParserInfo(this_class, nullptr, this_class->static_scope))) continue;
        if (parse_variable(ParserInfo(this_class, nullptr, this_class->static_scope))) continue;

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

    ScopePtr scope = Scope::create(Scope::FUNCTION, parser_info.cls->name);
    scope->upper_scope = parser_info.cls->scope;
    ParserInfo parser_info_header(parser_info.cls, nullptr, scope);

	// args
    if (!expect("(")) {
        index = start_idx;
        return false;
    }
    next(); // consume (

	// add pointer to self as arg
	auto class_type = parser_info.cls->type;
	auto pointer_type = std::make_shared<PointerType>(class_type);
	SourceLocation source_location_this(&source_file, 0, 1);
	auto this_var = VariableSymbol::create(source_location_this, pointer_type, "this", nullptr);
	scope->args.push_back(this_var->name);
	scope->variables[this_var->name] = this_var;
	this_var->scope = scope;

    std::vector<VarPtr> parameters;

    while (!expect(")")) {
        if (!parameters.empty()) {
            if (!expect(",")) {
                next();
                continue;
            }
            next(); // consume ,
        }

		size_t param_start_idx;
        // type
        TypePtr parsed_param_type = parse_type(parser_info_header);
        if (!parsed_param_type) {
            std::cout << "arg type error: " << peek().value << std::endl;
            next();
            continue;
        }

		TypePtr param_type;

		if (parsed_param_type->default_store_shared()) {
			param_type = std::make_shared<SharedType>(parsed_param_type);
		} else {
			param_type = parsed_param_type;
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

		SourceLocation source_location(&source_file, param_start_idx, index - 1);
		auto param = VariableSymbol::create(source_location, param_type, arg_name, initial_value);
		scope->args.push_back(arg_name);
		scope->variables[arg_name] = param;
		param->scope = scope;
        parameters.push_back(param);
    }
    next(); // consume )

    // body
    //std::cout << "parse body" << std::endl;

    TypePtr return_type = PrimitiveType::TYPE_VOID;
    COMPILER_ASSERT(return_type, "");

	SourceLocation source_location(&source_file, start_idx, index - 1);
    FuncPtr constructor_symbol = FunctionSymbol::create(source_location, return_type, parser_info.cls->name, parameters, scope, true);
    constructor_symbol->is_static = true; // treat constructor as static

    ParserInfo parser_info_body(parser_info.cls, constructor_symbol, scope);

    parse_body(parser_info_body, constructor_symbol);

    constructor_symbol->is_public = is_public;
    constructor_symbol->is_static = false;
    constructor_symbol->is_const = is_const;
	
    parser_info.cls->functions["(constructor)"] = constructor_symbol;

    return true;
}

bool SymbolBuilder::parse_function(ParserInfo parser_info) {
	size_t start_idx = index;
	
	bool is_override = false;
	
	if (expect("override")) {
		next(); // consume 'override'
		is_override = true;
	}

	// TODO: generic<T>

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

	ScopePtr scope = Scope::create(Scope::FUNCTION, "(currently parsed)");
    ScopePtr upper_scope = is_static ? parser_info.cls->static_scope : parser_info.cls->scope;
    scope->upper_scope = upper_scope;
    ParserInfo parser_info_header(parser_info.cls, nullptr, scope);

    // return type
	TypePtr return_type;

	if (!is_override) {
		return_type = parse_type(parser_info_header);
		
		if (!return_type) {
			index = start_idx;
			return false;
		}
	} else {
		return_type = nullptr;
	}

    // identifier
    if (!match(TokenType::IDENTIFIER)) {
        index = start_idx;
        return false;
    }
    std::string name = next().value;

    // args
    if (!expect("(")) {
        index = start_idx;
        return false;
    }
    next(); // consume (

	if (!is_static) {
		// add pointer to self as arg
		auto class_type = parser_info.cls->type;
		auto pointer_type = std::make_shared<PointerType>(class_type);
		SourceLocation source_location(&source_file, 0, 1);
		auto this_var = VariableSymbol::create(source_location, pointer_type, "this", nullptr);
		scope->args.push_back(this_var->name);
		scope->variables[this_var->name] = this_var;
		this_var->scope = scope;
	}

    std::vector<VarPtr> parameters;

    while (!expect(")")) {
        if (!parameters.empty()) {
            if (!expect(",")) {
                next();
                continue;
            }
            next(); // consume ,
        }

		size_t param_start_idx = index;
        // type
        TypePtr parsed_param_type = parse_type(parser_info_header);
        if (!parsed_param_type) {
            // TODO: add error
            std::cout << "arg type error: " << peek().value << std::endl;
            next();
            continue;
        }

		TypePtr param_type;

		if (parsed_param_type->default_store_shared()) {
			param_type = std::make_shared<SharedType>(parsed_param_type);
		} else {
			param_type = parsed_param_type;
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

		SourceLocation source_location(&source_file, param_start_idx, index - 1);
        VarPtr param = VariableSymbol::create(source_location, param_type, arg_name, initial_value);
        scope->args.push_back(arg_name);
		scope->variables[arg_name] = param;
        parameters.push_back(param);
    }
    next(); // consume )

    // body

    FuncPtr function_symbol;

	SourceLocation source_location(&source_file, start_idx, index - 1);
	
	if (is_override) {
		function_symbol = FunctionSymbol::create(source_location, name, parameters, scope, false);
	} else {
		function_symbol = FunctionSymbol::create(source_location, return_type, name, parameters, scope, false);
	}
	ParserInfo parser_info_body{parser_info.cls, function_symbol, scope};

    parse_body(parser_info_body, function_symbol);

    // result

    function_symbol->is_public = is_public;
    function_symbol->is_static = is_static;
    function_symbol->is_const = is_const;

    parser_info.cls->functions.insert(std::pair(function_symbol->name, function_symbol));

    return true;
}

bool SymbolBuilder::parse_operator(ParserInfo parser_info) {
	size_t start_idx = index;

	ScopePtr scope = Scope::create(Scope::FUNCTION, "(currently parsed)");
    ScopePtr upper_scope = parser_info.cls->scope;
    scope->upper_scope = upper_scope;
    ParserInfo parser_info_header(parser_info.cls, nullptr, scope);

    TypePtr return_type = parse_type(parser_info);
    if (!return_type) {
        index = start_idx;
        return false;
    }

	if (!expect("operator")) {
        index = start_idx;
        return false;
	}
	next(); // consume 'operator'

	std::string op;
	if (match(TokenType::SPECIAL)) {
	}
	op = next().value;

	//std::cout << op << std::endl;

	std::string name = "operator" + op;



	// parameters
	std::vector<VarPtr> parameters;
	
	// add pointer to self as arg
	auto class_type = parser_info.cls->type;
	auto pointer_type = std::make_shared<PointerType>(class_type);
	SourceLocation source_location_this(&source_file, 0, 1);
	auto this_var = VariableSymbol::create(source_location_this, pointer_type, "this", nullptr);
	scope->args.push_back(this_var->name);
	scope->variables[this_var->name] = this_var;
	this_var->scope = scope;

    if (!expect("(")) {
        index = start_idx;
        return false;
    }
    next(); // consume (

    while (!expect(")")) {
        if (!parameters.empty()) {
            if (!expect(",")) {
                next();
                continue;
            }
            next(); // consume ,
        }

		size_t param_start_idx = index;
        // type
        TypePtr arg_type = parse_type(parser_info_header);
        if (!arg_type) {
            // TODO: add error
            std::cerr << "arg type error: " << peek().value << std::endl;
            next();
            continue;
        }

        // name
        if (!match(TokenType::IDENTIFIER)) {
            std::cerr << "arg name error: " << peek().value << std::endl;
            next();
            continue;
        }
        std::string arg_name = next().value;

		SourceLocation source_location(&source_file, param_start_idx, index - 1);
        VarPtr param = VariableSymbol::create(source_location, arg_type, arg_name, nullptr);
        scope->args.push_back(arg_name);
		scope->variables[arg_name] = param;
        parameters.push_back(param);
    }
    next(); // consume )

	SourceLocation source_location(&source_file, start_idx, index - 1);

    // body
    FuncPtr function_symbol = FunctionSymbol::create(source_location, return_type, name, parameters, scope, false);
    ParserInfo parser_info_body(parser_info.cls, function_symbol, scope);

	parse_body(parser_info_body, function_symbol);

    // result

    function_symbol->is_public = true;
    function_symbol->is_static = false;
    function_symbol->is_const = false;

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

	TypePtr member_type;

	if (type->default_store_shared()) {
		member_type = std::make_shared<SharedType>(type);
	} else {
		member_type = type;
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
    
	SourceLocation source_location(&source_file, start_idx, index - 1);
    VarPtr variable_symbol = VariableSymbol::create(source_location, member_type, name, initial_value);
    variable_symbol->scope = scope;
    variable_symbol->is_public = is_public;
    variable_symbol->is_static = is_static;
    variable_symbol->is_const = is_const;
    
    scope->variables[name] = variable_symbol;

    return true;
}

void SymbolBuilder::parse_body(ParserInfo parser_info, FuncPtr function_symbol) {
	COMPILER_ASSERT(function_symbol, "");

	bool expect_expr;		// =
	bool expect_body;		// { }

	if (expect("=")) {
        next(); // consume '='

		if (expect("abstract")) {
            next(); // consume 'abstract'
            function_symbol->is_abstract = true;
			return;
        }

		expect_expr = true;
		expect_body = false;
	} else if (expect("{")) {
		next(); // consume '{'
		expect_expr = false;
		expect_body = true;
	} else {
		// error section
		if (expect("abstract")) {
            next(); // consume 'abstract'
            function_symbol->is_abstract = true;
			add_error(index, "missing '='", Error::ERROR);
			return;
        }

		expect_expr = false;
		expect_body = false;

		// recover to statement or file element
		size_t error_start_index = index;

		while (has_more_tokens()
				&& !expect("}")
				&& !peek().has_flag(
					TokenFlagBits::STMT_BEGIN
					| TokenFlagBits::FILE_ELEMENT_BEGIN
				)) {
			next();
		}

		add_error(error_start_index, "function " + function_symbol->name + " has no code", Error::ERROR);
	}

	if (expect_expr || !expect_body) {
		size_t error_start_index = index;
		
		auto expression = parse_expression(parser_info);

		if (expression) {
			std::shared_ptr<ReturnStatement> return_statement = std::make_shared<ReturnStatement>(expression);
			function_symbol->scope->body.push_back(return_statement);
		} else {
			while (has_more_tokens()
					&& !peek().has_flag(TokenFlagBits::FILE_ELEMENT_BEGIN)) {
				next();
			}

			add_error(error_start_index, "invalid expression", Error::ERROR);
		}

		if (expect_expr) return;
    }
	
	// try parsing a whole body as final option
	while (!expect("}")) {
		StmtVec statements = parse_statement(parser_info);

		if (statements.empty()) {
			size_t error_start_index = index;

			while (has_more_tokens()
					&& !expect("}")
					&& !peek().has_flag(TokenFlagBits::STMT_BEGIN)) {
				next();
			}

			add_error(error_start_index, "invalid statement in function " + function_symbol->head_to_string(), Error::ERROR);
		} else {
			for (auto st : statements) {
				COMPILER_ASSERT(st, "");
				st->is_volatile = false;
				COMPILER_ASSERT(function_symbol->scope, "");
				function_symbol->scope->body.push_back(st);
				//std::cout << st->to_string() << std::endl;
			}
		}
	}
	next(); // consume '}'
}

}