#pragma once

#include "token.hpp"
#include "symbol_table.hpp"
#include "class_symbol.hpp"
#include <vector>
#include <memory>
#include "forward_declarations.hpp"

namespace cmp {

struct ParserInfo {
    const ClassPtr cls;
    const FuncPtr func;
    const ScopePtr scope;
};

class BlockStatement;

class SymbolBuilder {
public:
    SymbolBuilder(const std::vector<Token>& tokens, SymbolTable& symbol_table);

    void parse_class();

    bool parse_constructor(ParserInfo parser_info);
    bool parse_function(ParserInfo parser_info);
    bool parse_variable(ParserInfo parser_info);
    
    TypePtr parse_type(ParserInfo& parser_info);
    TypePtr parse_primitive_type(ParserInfo& parser_info);
    TypePtr parse_class_type(ParserInfo& parser_info);
    TypePtr parse_tuple_type(ParserInfo& parser_info);

    // statements
    StmtPtr parse_statement(ParserInfo& parser_info);

    StmtPtr parse_assign(ParserInfo& parser_info);
    StmtPtr parse_declare_statement(ParserInfo& parser_info);

    std::shared_ptr<BlockStatement> parse_block(ParserInfo& parser_info);

    StmtPtr parse_if(ParserInfo& parser_info);
    StmtPtr parse_for(ParserInfo& parser_info);
    StmtPtr parse_while(ParserInfo& parser_info);

    StmtPtr parse_expression_statement(ParserInfo& parser_info);

    StmtPtr parse_return(ParserInfo& parser_info);
    
    // expressions
    ExprPtr parse_expression(ParserInfo& parser_info);

    ExprPtr parse_or(ParserInfo& parser_info);
    ExprPtr parse_and(ParserInfo& parser_info);
    ExprPtr parse_equality(ParserInfo& parser_info);
    ExprPtr parse_comparison(ParserInfo& parser_info);
    ExprPtr parse_add(ParserInfo& parser_info);
    ExprPtr parse_mul(ParserInfo& parser_info);
    ExprPtr parse_access(ParserInfo& parser_info);
    
    ExprPtr parse_primary(ParserInfo& parser_info);
    ExprPtr parse_call(ParserInfo& parser_info);

    ExprPtr parse_tuple(ParserInfo& parser_info);

private:
    const std::vector<Token>& tokens;
    SymbolTable& symbol_table;
    size_t index;

    bool has_more_tokens() const;

    const Token& peek() const;
    const Token& next();

    bool match(TokenType type);
    bool expect(const std::string& value);
};

}