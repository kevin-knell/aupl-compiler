// symbol_builder_statement.cpp
#include "symbol_builder.hpp"
#include "declare_statement.hpp"
#include "assign_statement.hpp"
#include "expression_statement.hpp"
#include "return_statement.hpp"
#include "label_statement.hpp"
#include "label.hpp"
#include "conditional_jump_statement.hpp"
#include "block_statement.hpp"
#include <iostream>

namespace cmp {


std::vector<StmtPtr> SymbolBuilder::parse_statement(ParserInfo& parser_info) {
    std::vector<StmtPtr> result;
    auto if_stmts = parse_if(parser_info); if (!if_stmts.empty()) return if_stmts;
    auto declare_stmts = parse_declare_statement(parser_info); if (!declare_stmts.empty()) return declare_stmts;
    auto assign_stmts = parse_assign(parser_info); if (!assign_stmts.empty()) return assign_stmts;
    auto return_stmts = parse_return(parser_info); if (!return_stmts.empty()) return return_stmts;
    auto expr_stmts = parse_expression_statement(parser_info); if (!expr_stmts.empty()) return expr_stmts;
    return result;
}

std::vector<StmtPtr> SymbolBuilder::parse_assign(ParserInfo& parser_info) {
    // Save current index to backtrack if no match
    size_t start_idx = index;

    // Expect identifier for variable name
    auto expr_left = parse_expression(parser_info);
    if (!expr_left) {
        index = start_idx;
        return {};
    }

    // Expect '=' operator
    if (!expect("=")) {
        index = start_idx;
        return {};
    }
    next(); // consume '='

    // Parse the assigned expression
    auto expr_right = parse_expression(parser_info);
    if (!expr_right) {
        index = start_idx;
        return {};
    }

    return { std::make_shared<AssignmentStatement>(expr_left, expr_right) };
}

std::vector<StmtPtr> SymbolBuilder::parse_declare_statement(ParserInfo& parser_info) {
    // Save current index to backtrack if no match
    size_t start_idx = index;

    TypePtr type = parse_type(parser_info);
    if (!type) {
        index = start_idx;
        return {};
    }

    // Expect identifier for variable name
    if (!match(TokenType::IDENTIFIER)) {
        index = start_idx;
        return {};
    }
    std::string var_name = next().value;

    std::shared_ptr<Expression> expr = nullptr;
    // Expect '=' operator
    if (expect("=")) {
        next(); // consume '='
        // Parse the assigned expression
        expr = parse_expression(parser_info);
        if (!expr) {
            std::cout << "error: invalid expression" << std::endl;
        }
    }
    VarPtr var = std::make_shared<VariableSymbol>(type, var_name, expr);
    parser_info.scope->variables[var_name] = var;
    var->scope = parser_info.scope;
    return { std::make_shared<DeclareStatement>(var) };
}

std::shared_ptr<BlockStatement> SymbolBuilder::parse_block(ParserInfo& parser_info) {
    size_t start_idx = index;

    if (!expect("{")) {
        index = start_idx;
        return nullptr;
    }
    next(); // consume {

    ScopePtr scope = std::make_shared<Scope>(Scope::FUNCTION_SUB);
    scope->upper_scope = parser_info.scope;
    parser_info.scope->lower_scopes.push_back(std::weak_ptr<Scope>(scope));
    scope->name = "block";

    auto parser_info_sub = ParserInfo{.cls = parser_info.cls, .func = parser_info.func, .scope = scope};

    while (!expect("}")) {
        bool is_volatile = false;
        if (expect("volatile")) {
            is_volatile = true;
            next(); // consume 'volatile'
        }
        StmtVec stmts = parse_statement(parser_info_sub);
        if (stmts.empty()) {
            index = start_idx;
            return nullptr;
        }
		for (auto st : stmts) {
        	st->is_volatile = is_volatile;
        	scope->body.push_back(st);
		}
    }
    next(); // consume }

    return std::make_shared<BlockStatement>(scope);
}

std::vector<StmtPtr> SymbolBuilder::parse_if(ParserInfo& parser_info) {
    size_t start_idx = index;

    if (!expect("if")) {
        index = start_idx;
        return {};
    }
    next(); // consume if

    std::cout << "if" << std::endl;

    ExprPtr condition_expr = parse_expression(parser_info);
    if (!condition_expr) {
        index = start_idx;
        return {};
    }

	std::string if_ret_name = parser_info.scope->get_label_name("if_return");
	std::string if_name = parser_info.scope->get_label_name("if");
	std::string else_name = parser_info.scope->get_label_name("else");

    std::shared_ptr<BlockStatement> if_block = parse_block(parser_info);
    std::shared_ptr<Label> if_label = std::make_shared<Label>(if_block->scope, if_name);

    std::shared_ptr<BlockStatement> else_block = nullptr;
    std::shared_ptr<Label> else_label = nullptr;

    if (expect("else")) {
        next(); // consume else

        else_block = parse_block(parser_info);

        if (!else_block) {
            index = start_idx;
            return {};
        }
        else_label = std::make_shared<Label>(else_block->scope, else_name);
    }

	StmtVec result;
	result.push_back(std::make_shared<ConditionalJumpStatement>(condition_expr, if_label, else_label));

	std::shared_ptr<LabelStatement> return_label_stmt = std::make_shared<LabelStatement>(if_ret_name);
	result.push_back(return_label_stmt);

	std::shared_ptr<Label> return_label = std::make_shared<Label>(parser_info.scope, if_ret_name, return_label_stmt);

	if_block->scope->body.push_back(
		std::make_shared<ConditionalJumpStatement>(nullptr, return_label, nullptr)
	);

	else_block->scope->body.push_back(
		std::make_shared<ConditionalJumpStatement>(nullptr, return_label, nullptr)
	);

    return result;
}

std::vector<StmtPtr> SymbolBuilder::parse_for(ParserInfo& parser_info) {
    (void)parser_info;
    //size_t start_idx = index;
    return {};
}

std::vector<StmtPtr> SymbolBuilder::parse_while(ParserInfo& parser_info) {
    (void)parser_info;
    //size_t start_idx = index;
    return {};
}

std::vector<StmtPtr> SymbolBuilder::parse_return(ParserInfo& parser_info) {
    size_t start_idx = index;

    // Look for 'return' keyword
    if (!expect("return")) {
        index = start_idx;
        return {};
    }
    next(); // consume 'return'

    // Parse the returned expression, or nullptr for empty return
    auto expr = parse_expression(parser_info);

    return { std::make_shared<ReturnStatement>(expr) };
}

std::vector<StmtPtr> SymbolBuilder::parse_expression_statement(ParserInfo& parser_info) {
    size_t start_idx = index;

    // Try parsing an expression
    auto expr = parse_expression(parser_info);
    if (!expr) {
        index = start_idx;
        return {};
    }

    return { std::make_shared<ExpressionStatement>(expr) };
}

}