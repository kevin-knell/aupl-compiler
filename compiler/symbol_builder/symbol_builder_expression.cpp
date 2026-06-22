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

#define EXPR_DEBUG_PRINT(m_text) //std::cout << (m_text) << ": " << peek().value << std::endl;

#define RETURN_IF_NOT(m_value)		\
	do {							\
		if (!(m_value)) {			\
    	    index = start_idx;		\
    	    return nullptr;			\
    	}							\
	} while(0)

#define ERROR_IF_NOT(m_value, m_message)		\
	do {							\
		if (!(m_value)) {			\
    	    add_error(0, m_message, Error::CRITICAL);		\
    	    abort();			\
    	}							\
	} while(0)

#define TRY_CONSUME(m_value)		\
	do {							\
		if (!expect(m_value)) {		\
    	    index = start_idx;		\
    	    return nullptr;			\
    	}							\
    	next();						\
	} while(0)

namespace cmp {

ExprPtr SymbolBuilder::parse_expression(ParserInfo& parser_info) {
	EXPR_DEBUG_PRINT("start parsing expr");

	if (!peek().has_flag(TokenFlagBits::EXPR_BEGIN)) {
		size_t error_start = index;
		
		recover_to_expr();

		add_error(error_start, "Invalid start for expression: " + tokens[error_start].value, Error::ERROR);

		if (!has_more_tokens()) {
			return nullptr;
		}
	}

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
			EXPR_DEBUG_PRINT("expr: " + left->to_string() + " + ...");
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

// . []
ExprPtr SymbolBuilder::parse_postfix(ParserInfo& parser_info) {
	size_t start_idx = index;

    ExprPtr expr = parse_primary(parser_info);
    if (!expr) return nullptr;

    while (true) {
        if (expect(".")) {
            next(); // consume '.'

			if (peek().is_new_line) {
				add_error(start_idx, "newline after '.'", Error::ERROR);
				break;
			}

            ExprPtr right = parse_primary(parser_info);
            if (!right) {
				add_error(start_idx, "expected call or variable", Error::ERROR);
				recover_to_expr();
				continue;
			}

            if (right->get_kind() == Expression::CALL) {
                auto call_expr = std::dynamic_pointer_cast<CallExpression>(right);
                call_expr->obj_expr = expr;
                expr = call_expr;
            } else if (right->get_kind() == Expression::VARIABLE) {
                auto var_expr = std::dynamic_pointer_cast<VariableExpression>(right);
                var_expr->obj_expr = expr;
                expr = var_expr;
            } else {
				add_error(start_idx, "invalid member access: " + right->to_string(), Error::ERROR);
            }
        } else if (expect("[")) {
			if (peek().is_new_line) {
				add_error(start_idx, "newline before '['", Error::ERROR);
			}
            next(); // consume '['

            ExprPtr index_expr = parse_expression(parser_info);
			ERROR_IF_NOT(index_expr, "missing index expression");

			ERROR_IF_NOT(expect("]"), "expected ']'");
            next(); // consume ']'

            expr = std::make_shared<IndexExpression>(expr, index_expr);
        } else {
            break;
        }
    }

    return expr;
}

ExprPtr SymbolBuilder::parse_primary(ParserInfo& parser_info) {
	EXPR_DEBUG_PRINT("parse primary");
	if (auto call_expr = parse_call(parser_info)) return call_expr;
    if (auto tuple_expr = parse_tuple(parser_info)) return tuple_expr;
    if (auto init_list_expr = parse_initializer_list(parser_info)) return init_list_expr;
	
	if (expect("\\")) {
		ExprPtr result = parse_node_composition(parser_info);
		return result;
	}
	
	EXPR_DEBUG_PRINT("parse single token expression");

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

    return nullptr;
}

ExprPtr SymbolBuilder::parse_call(ParserInfo& parser_info) {
    size_t start_idx = index;

	RETURN_IF_NOT(match(TokenType::IDENTIFIER));
    std::string name = next().value;

	TRY_CONSUME("(");

	EXPR_DEBUG_PRINT("parse call");

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
    size_t start_idx = index;

	TRY_CONSUME("(");

	EXPR_DEBUG_PRINT("parse (...)");

    ExprVec expressions;

    while (!expect(")")) {
        if (!expressions.empty()) {
			if (expect(",")) {
				next(); // consume ,
			} else {
				index = start_idx;
				return nullptr;
			}
        }

        ExprPtr t = parse_expression(parser_info);

        if (!t) {
            index = start_idx;
            return nullptr;
        }
        expressions.push_back(t);
    }
    next(); // consume )

	if (expressions.size() == 1) {
		return expressions.front();
	}

    return std::make_shared<TupleExpression>(expressions);
}

ExprPtr SymbolBuilder::parse_initializer_list(ParserInfo &parser_info) {
    size_t start_idx = index;

	TRY_CONSUME("{");

    ExprVec expressions;

    while (!expect("}")) {
        if (!expressions.empty()) {
			if (expect(",")) {
				next(); // consume ,
			} else {
				index = start_idx;
				return nullptr;
			}
        }

        ExprPtr t = parse_expression(parser_info);
        if (!t) {
            index = start_idx;
            return nullptr;
        }
        expressions.push_back(t);
    }
    next(); // consume }

    return std::make_shared<TupleExpression>(expressions);
}

ExprPtr SymbolBuilder::parse_node_composition(ParserInfo &parser_info) {
	size_t start_idx = index;

	TRY_CONSUME("\\");

	EXPR_DEBUG_PRINT("parse node composition");

	ERROR_IF_NOT(match(TokenType::IDENTIFIER), "identifier expected after '\\'");
	std::string node_name = next().value;

	// parse args / values
	ERROR_IF_NOT(expect("("), "'(' expected after \\Node");
	next(); // consume '('

	ExprVec args;

	while(!expect(")")) {
		if (!args.empty()) {
			ERROR_IF_NOT(expect(","), "',' expected in \\Node(...)");
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
	ERROR_IF_NOT(expect("{"), "'{' expected after \\Node()");
	next(); // consume '{'

	ExprVec content;

	while(!expect("}")) {
		if (!content.empty()) {
			ERROR_IF_NOT(expect(","), "',' expected in \\Node() {...}");
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

void SymbolBuilder::recover_to_expr() {
	while (has_more_tokens()
			&& !peek().has_flag(TokenFlagBits::CLOSING_PAREN)) {
		next();

		if (peek().has_flag(TokenFlagBits::EXPR_BEGIN)
				|| peek().has_flag(TokenFlagBits::STMT_BEGIN)) {
			break;
		}
	}
}

}