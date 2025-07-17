// symbol_builder_expression.cpp
#include "symbol_builder.hpp"
#include "variable_expression.hpp"
#include "binary_op_expression.hpp"
#include "access_expression.hpp"
#include "call_expression.hpp"
#include "load_const_expression.hpp"
#include "primitive_type.hpp"
#include "tuple_expression.hpp"
#include <iostream>
#include <cassert>

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
    ExprPtr left = parse_equality(parser_info);
    while (expect("and")) {
        next(); // consume 'and'
        ExprPtr right = parse_equality(parser_info);
        if (!right) return nullptr;
        left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::AND);
    }
    return left;
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
    ExprPtr left = parse_access(parser_info);
    if (!left) return nullptr;

    while (expect("*") || expect("/") || expect("%")) {
        if (expect("*")) {
            next(); // consume *
            ExprPtr right = parse_access(parser_info);
            if (!right) return nullptr;
            left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::MUL);
        } else if (expect("/")) {
            next(); // consume /
            ExprPtr right = parse_access(parser_info);
            if (!right) return nullptr;
            left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::DIV);
        } else if (expect("%")) {
            next(); // consume %
            ExprPtr right = parse_access(parser_info);
            if (!right) return nullptr;
            left = std::make_shared<BinaryExpression>(left, right, BinaryExpression::OPERATOR::MOD);
        }
    }
    return left;
}

ExprPtr SymbolBuilder::parse_access(ParserInfo& parser_info) {
    ExprPtr left = parse_primary(parser_info);
    if (!left) return nullptr;

    while (expect(".")) {
        next(); // consume .
        ExprPtr right = parse_primary(parser_info);
        if (!right) return nullptr;
        left = std::make_shared<AccessExpression>(left, right);
    }

    return left;
}

ExprPtr SymbolBuilder::parse_primary(ParserInfo& parser_info) {
    if (auto call_expr = parse_call(parser_info)) return call_expr;
    if (auto tuple_expr = parse_tuple(parser_info)) return tuple_expr;

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
        vm::Value8* value8 = new vm::Value8();
        value8->i64 = std::stoi(next().value);
        auto value = reinterpret_cast<vm::Value*>(value8);
        auto load_expr = std::make_shared<LoadConstExpression>(PrimitiveType::TYPE_INT, value);
        return load_expr;
    }

    if (match(TokenType::IDENTIFIER)) {
        std::string name = next().value;
        ExprPtr result = std::make_shared<VariableExpression>(name);
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

    return std::make_shared<CallExpression>(name, args);
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

}