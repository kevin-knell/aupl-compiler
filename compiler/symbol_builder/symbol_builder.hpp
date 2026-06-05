#pragma once

#include "token.hpp"
#include "symbol_table.hpp"
#include "class_symbol.hpp"
#include <vector>
#include <memory>
#include "forward_declarations.hpp"

namespace cmp {

struct ParserInfo {
	SymbolTable& symbol_table;
    const ClassPtr cls;
    const FuncPtr func;
    const ScopePtr scope;
};

class SymbolBuilder {
public:
    SymbolBuilder(
		const SourceFile& source_file,
		SymbolTable& symbol_table);

    void parse_class();

    bool parse_constructor(ParserInfo parser_info);
    bool parse_function(ParserInfo parser_info);
    bool parse_operator(ParserInfo parser_info);
    bool parse_variable(ParserInfo parser_info);

	void parse_body(ParserInfo parser_info, FuncPtr function_symbol);
    
    TypePtr parse_type(ParserInfo& parser_info);
    TypePtr parse_base_type(ParserInfo& parser_info);
	TypePtr parse_native_type(ParserInfo& parser_info);
    TypePtr parse_primitive_type(ParserInfo& parser_info);
    TypePtr parse_class_type(ParserInfo& parser_info);
    TypePtr parse_tuple_type(ParserInfo& parser_info);
    TypePtr parse_array_type(ParserInfo& parser_info);

    // statements
    std::vector<StmtPtr> parse_statement(ParserInfo& parser_info);

    std::vector<StmtPtr> parse_assign(ParserInfo& parser_info);
    std::vector<StmtPtr> parse_declare_statement(ParserInfo& parser_info);

    ScopePtr parse_block(ParserInfo& parser_info, const std::string& block_name);

    std::vector<StmtPtr> parse_if(ParserInfo& parser_info);
    std::vector<StmtPtr> parse_for(ParserInfo& parser_info);
    std::vector<StmtPtr> parse_while(ParserInfo& parser_info);

    std::vector<StmtPtr> parse_expression_statement(ParserInfo& parser_info);

    std::vector<StmtPtr> parse_return(ParserInfo& parser_info);
    
    // expressions
    ExprPtr parse_expression(ParserInfo& parser_info);

    ExprPtr parse_or(ParserInfo& parser_info);
    ExprPtr parse_and(ParserInfo& parser_info);
    ExprPtr parse_not(ParserInfo& parser_info);
    ExprPtr parse_equality(ParserInfo& parser_info);
    ExprPtr parse_comparison(ParserInfo& parser_info);
    ExprPtr parse_add(ParserInfo& parser_info);
    ExprPtr parse_mul(ParserInfo& parser_info);

    //ExprPtr parse_access(ParserInfo& parser_info);
    //ExprPtr parse_index(ParserInfo& parser_info);
    ExprPtr parse_postfix(ParserInfo& parser_info);
    
    ExprPtr parse_primary(ParserInfo& parser_info);
    ExprPtr parse_call(ParserInfo& parser_info);

    ExprPtr parse_tuple(ParserInfo& parser_info);
    ExprPtr parse_initializer_list(ParserInfo& parser_info);

private:
	const SourceFile& source_file;
    const std::vector<Token>& tokens;
    SymbolTable& symbol_table;
	std::string class_name;
    size_t index;

    bool has_more_tokens() const;

    const Token& peek() const;
    const Token& next();

    bool match(TokenType type);
    bool expect(const std::string& value);

	Error& add_error(size_t start_idx, const std::string message, Error::Level level);
};

}