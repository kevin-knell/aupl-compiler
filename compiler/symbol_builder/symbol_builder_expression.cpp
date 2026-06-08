// symbol_builder_expression.cpp
#include "symbol_builder.hpp"
#include "variable_expression.hpp"
#include "unary_op_expression.hpp"
#include "binary_op_expression.hpp"
#include "call_expression.hpp"
#include "load_const_expression.hpp"
#include "primitive_type.hpp"
#include "tuple_expression.hpp"
#include "string.hpp"
#include "string_literal_expression.hpp"
#include "shared_type.hpp"
#include "index_expression.hpp"
#include "node_composition_expression.hpp"
#include <iostream>
#include "compiler_error.hpp"

namespace cmp {

ExprPtr SymbolBuilder::parse_expression(ParserInfo& parser_info) {
    return parse_or(parser_info);
}

// or / and
ExprPtr SymbolBuilder::parse_or(ParserInfo& parser_info) {
    ExprPtr left = parse_and(parser_info);
    while (expect("or")) {
        next(); // consume 'or'
        ExprPtr right = parse_and(parser_info);
        if (!right) return nullptr;
        left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::OR);
    }
    return left;
}

ExprPtr SymbolBuilder::parse_and(ParserInfo& parser_info) {
    ExprPtr left = parse_not(parser_info);
    while (expect("and")) {
        next(); // consume 'and'
        ExprPtr right = parse_not(parser_info);
        if (!right) return nullptr;
        left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::AND);
    }
    return left;
}

ExprPtr SymbolBuilder::parse_not(ParserInfo& parser_info) {
    if (expect("not")) {
        next(); // consume 'not'
        ExprPtr expr = parse_equality(parser_info);
        if (!expr) return nullptr;
        return std::make_shared<UnaryExpression>(expr, UnaryExpression::OPERATOR::NOT);
    }
    return parse_equality(parser_info);
}

ExprPtr SymbolBuilder::parse_equality(ParserInfo& parser_info) {
    ExprPtr left = parse_comparison(parser_info);
    while (expect("==") || expect("!=")) {
        if (expect("==")) {
            next();
            ExprPtr right = parse_comparison(parser_info);
            if (!right) return nullptr;
            left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::EQ);
        } else if (expect("!=")) {
            next();
            ExprPtr right = parse_comparison(parser_info);
            if (!right) return nullptr;
            left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::NEQ);
        }
    }
    return left;
}

// == !=
ExprPtr SymbolBuilder::parse_comparison(ParserInfo& parser_info) {
    ExprPtr left = parse_add(parser_info);
    while (expect("<") || expect(">") || expect("<=") || expect(">=")) {
        if (expect("<")) {
            next();
            ExprPtr right = parse_add(parser_info);
            if (!right) return nullptr;
            left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::LT);
        } else if (expect(">")) {
            next();
            ExprPtr right = parse_add(parser_info);
            if (!right) return nullptr;
            left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::GT);
        } else if (expect("<=")) {
            next();
            ExprPtr right = parse_add(parser_info);
            if (!right) return nullptr;
            left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::LE);
        } else if (expect(">=")) {
            next();
            ExprPtr right = parse_add(parser_info);
            if (!right) return nullptr;
            left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::GE);
        }
    }
    return left;
}

// + -
ExprPtr SymbolBuilder::parse_add(ParserInfo& parser_info) {
    ExprPtr left = parse_mul(parser_info);
    if (!left) return nullptr;

    while (expect("+") || expect("-")) {
		if (expect("+")){
        	next(); // consume +
        	ExprPtr right = parse_mul(parser_info);
        	if (!right) return nullptr;
        	left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::ADD);
		} else if (expect("-")){
        	next(); // consume -
        	ExprPtr right = parse_mul(parser_info);
        	if (!right) return nullptr;
        	left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::SUB);
		}
    }

    return left;
}

// * / %
ExprPtr SymbolBuilder::parse_mul(ParserInfo& parser_info) {
    ExprPtr left = parse_postfix(parser_info);
    if (!left) return nullptr;

    while (expect("*") || expect("/") || expect("%")) {
        if (expect("*")) {
            next(); // consume *
            ExprPtr right = parse_postfix(parser_info);
            if (!right) return nullptr;
            left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::MUL);
        } else if (expect("/")) {
            next(); // consume /
            ExprPtr right = parse_postfix(parser_info);
            if (!right) return nullptr;
            left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::DIV);
        } else if (expect("%")) {
            next(); // consume %
            ExprPtr right = parse_postfix(parser_info);
            if (!right) return nullptr;
            left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::MOD);
        }
    }
    return left;
}

/*
ExprPtr SymbolBuilder::parse_access(ParserInfo& parser_info) {
    ExprPtr left = parse_index(parser_info);
    if (!left) return nullptr;

    while (expect(".")) {
        next(); // consume .
        ExprPtr right = parse_primary(parser_info);
        if (!right) return nullptr;

		if (right->get_kind() == Expression::CALL) {
			auto call_expr = std::dynamic_pointer_cast<CallExpression>(right);
			call_expr->obj_expr = left;
			left = call_expr;
			continue;
		} else if (right->get_kind() == Expression::VARIABLE) {
			auto var_expr = std::dynamic_pointer_cast<VariableExpression>(right);
			var_expr->obj_expr = left;
			left = var_expr;
			continue;
		} else {
			std::cerr << "unknown access expression: " << right->to_string() << std::endl;
			exit(1);
		}
    }

    return left;
}
*/

/*
ExprPtr SymbolBuilder::parse_index(ParserInfo &parser_info) {
    ExprPtr left = parse_access(parser_info);
    if (!left) return nullptr;

	if (!expect("[")) {
		return left;
	}
	next(); // consume '['

	ExprPtr index_expr = parse_expression(parser_info);
	if (!index_expr) {
		std::cerr << "error! no index expression!" << std::endl;
		exit(1);
	}

	if (!expect("]")) {
		std::cerr << "error! ] expected in indexing operator!" << std::endl;
		exit(1);
	}
	next(); // consume ']'

	std::cout << left->to_string() << "[" << index_expr->to_string() << "]" << std::endl;

	exit(1);

	return ExprPtr();
}
*/

ExprPtr SymbolBuilder::parse_postfix(ParserInfo& parser_info) {
    ExprPtr expr = parse_primary(parser_info);
    if (!expr) return nullptr;

    while (true) {
        if (expect(".")) {
            next(); // consume '.'

            ExprPtr right = parse_primary(parser_info);
            if (!right) return nullptr;

            if (right->get_kind() == Expression::CALL) {
                auto call_expr = std::dynamic_pointer_cast<CallExpression>(right);
                call_expr->obj_expr = expr;
                expr = call_expr;
            } else if (right->get_kind() == Expression::VARIABLE) {
                auto var_expr = std::dynamic_pointer_cast<VariableExpression>(right);
                var_expr->obj_expr = expr;
                expr = var_expr;
            } else {
                std::cerr << "invalid member access: " << right->to_string() << std::endl;
                return nullptr;
            }
        } else if (expect("[")) {
            next(); // consume '['

            ExprPtr index_expr = parse_expression(parser_info);
            if (!index_expr) {
                std::cerr << "missing index expression" << std::endl;
                exit(1);
            }

            if (!expect("]")) {
                std::cerr << "expected ']'" << std::endl;
                exit(1);
            }
            next(); // consume ']'

            expr = std::make_shared<IndexExpression>(expr, index_expr);
        } else {
            break;
        }
    }

    return expr;
}

ExprPtr SymbolBuilder::parse_primary(ParserInfo& parser_info) {
	if (auto call_expr = parse_call(parser_info)) return call_expr;
    if (auto tuple_expr = parse_tuple(parser_info)) return tuple_expr;
    if (auto init_list_expr = parse_initializer_list(parser_info)) return init_list_expr;
	
    if (match(TokenType::INT_LITERAL)) {
        vm::Value8* value8 = new vm::Value8();
        value8->i64 = std::stoi(next().value);
        auto value = reinterpret_cast<vm::Value*>(value8);
        auto load_expr = std::make_shared<LoadConstExpression>(PrimitiveType::TYPE_INT, value);
        return load_expr;
    }

    if (match(TokenType::DOUBLE_LITERAL)) {
        vm::Value8* value8 = new vm::Value8();
        value8->f64 = std::stod(next().value);
        auto value = reinterpret_cast<vm::Value*>(value8);
        auto load_expr = std::make_shared<LoadConstExpression>(PrimitiveType::TYPE_FLOAT, value);
        return load_expr;
    }

	if (expect("true")) {
		next(); // consume true
		vm::Value* value = new vm::Value();
		value->b = true; // true
		auto load_expr = std::make_shared<LoadConstExpression>(PrimitiveType::TYPE_BOOL, value);
		return load_expr;
	}

	if (expect("false")) {
		next(); // consume false
		vm::Value* value = new vm::Value();
		value->b = false; // false
		auto load_expr = std::make_shared<LoadConstExpression>(PrimitiveType::TYPE_BOOL, value);
		return load_expr;
	}

    if (match(TokenType::STRING_LITERAL)) {
		std::string value = next().value;
		auto it = symbol_table.native_types.find("String");
		COMPILER_ASSERT(it != symbol_table.native_types.end(), "String Type not found");
		auto string_type = it->second;
		auto result = std::make_shared<StringLiteralExpression>(string_type, value.substr(1, value.size() - 2));
		return result;
    }

	if (expect("this")) {
		next(); // consume 'this'
        ExprPtr result = std::make_shared<VariableExpression>(parser_info.cls->scope->variables["this"]);
        return result;
	}

    if (match(TokenType::IDENTIFIER)) {
        std::string name = next().value;
        ExprPtr result = std::make_shared<VariableExpression>(name);
        return result;
    }

	if (expect("\\")) {
		ExprPtr result = parse_node_composition(parser_info);
		return result;
	}

    return nullptr;
}

ExprPtr SymbolBuilder::parse_call(ParserInfo& parser_info) {
    (void)parser_info;
    size_t start_idx = index;

    if (!match(TokenType::IDENTIFIER)) {
        index = start_idx;
        return nullptr;
    }
    std::string name = next().value;

    if (!expect("(")) {
        index = start_idx;
        return nullptr;
    }
    next(); // consume (

    // TODO: parse args
    std::vector<ExprPtr> args;
    while (!expect(")")) {
        ExprPtr arg = parse_expression(parser_info);
        if (arg) {
            args.push_back(arg);
            if (expect(",")) {
                next(); // consume ,
            }
        } else {
            std::cerr << "invalid arg in " << name << std::endl;
            next();
        }
    }
    next(); // consume )

	std::shared_ptr<CallExpression> call_expr = std::make_shared<CallExpression>(name, args, nullptr);

	call_expr->source_location = SourceLocation(&source_file, start_idx, index);

    return call_expr;
}

ExprPtr SymbolBuilder::parse_tuple(ParserInfo &parser_info) {
    size_t idx = index;

    if (!expect("(")) {
		index = idx;
        return nullptr;
    }
    next(); // consume (

    ExprVec expressions;

    while (!expect(")")) {
        if (!expressions.empty()) {
			if (expect(",")) {
				next(); // consume ,
			} else {
				index = idx;
				return nullptr;
			}
        }

        ExprPtr t = parse_expression(parser_info);
        if (!t) {
            index = idx;
            return nullptr;
        }
        expressions.push_back(t);
    }
    next(); // consume )

    return std::make_shared<TupleExpression>(expressions);
}

ExprPtr SymbolBuilder::parse_initializer_list(ParserInfo &parser_info) {
    size_t idx = index;

    if (!expect("{")) {
		index = idx;
        return nullptr;
    }
    next(); // consume {

    ExprVec expressions;

    while (!expect("}")) {
        if (!expressions.empty()) {
			if (expect(",")) {
				next(); // consume ,
			} else {
				index = idx;
				return nullptr;
			}
        }

        ExprPtr t = parse_expression(parser_info);
        if (!t) {
            index = idx;
            return nullptr;
        }
        expressions.push_back(t);
    }
    next(); // consume }

    return std::make_shared<TupleExpression>(expressions);
}

ExprPtr SymbolBuilder::parse_node_composition(ParserInfo &parser_info) {
	size_t start_idx = index;
	
	if (!expect("\\")) {
		return nullptr;
	}
	next(); // consume '\'

	if (!match(TokenType::IDENTIFIER)) {
		SourceLocation source_location(&source_file, start_idx, index);
		symbol_table.add_error(source_location, "identifier expected after '\\'", Error::ERROR);

		// TODO: recover

		return nullptr;
	}
	std::string node_name = next().value;

	// parse args / values
	if (!expect("(")) {
		SourceLocation source_location(&source_file, start_idx, index);
		symbol_table.add_error(source_location, "'(' expected after \\Node", Error::ERROR);
	}
	next(); // consume '('

	ExprVec args;

	while(!expect(")")) {
		if (!args.empty()) {
			if (!expect(",")) {
				SourceLocation source_location(&source_file, start_idx, index);
				symbol_table.add_error(source_location, "',' expected in \\Node(...)", Error::ERROR);
			}
			next(); // consume ','
		}

		// TODO: add '=' option

		ExprPtr expr = parse_expression(parser_info);

		if (!expr) {
			SourceLocation source_location(&source_file, start_idx, index);
			symbol_table.add_error(source_location, "invalid expression in \\Node(...)", Error::ERROR);
			abort(); // TODO: recover properly
			continue;
		}

		args.push_back(expr);
	}
	next(); // consume ')'

	// parse content
	if (!expect("{")) {
		SourceLocation source_location(&source_file, start_idx, index);
		symbol_table.add_error(source_location, "'{' expected after \\Node()", Error::ERROR);
	}
	next(); // consume '{'

	ExprVec content;

	while(!expect("}")) {
		if (!content.empty()) {
			if (!expect(",")) {
				SourceLocation source_location(&source_file, start_idx, index);
				symbol_table.add_error(source_location, "',' expected in \\Node() {...}", Error::ERROR);
			}
			next(); // consume ','
		}

		ExprPtr expr = parse_expression(parser_info);

		if (!expr) {
			SourceLocation source_location(&source_file, start_idx, index);
			symbol_table.add_error(source_location, "invalid expression in \\Node() {...}", Error::ERROR);
			abort(); // TODO: recover properly
			continue;
		}

		content.push_back(expr);
	}
	next(); // consume '}'

	return std::make_shared<NodeCompositionExpression>(node_name, args, content);
}

}