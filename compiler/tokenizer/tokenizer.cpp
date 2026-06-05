// tokenizer.cpp
#include "tokenizer.hpp"
#include <regex>
#include <unordered_set>
#include <string>
#include <vector>
#include <iostream>

#define COMMA ,

#define KEYWORDS(x, D) \
		/* class header */ \
        x(class, NONE) D \
		x(interface, NONE) D \
		/* class-like definitions */ \
		x(singleton, FILE_ELEMENT_BEGIN) D \
		x(struct, FILE_ELEMENT_BEGIN) D \
		x(impl, FILE_ELEMENT_BEGIN) D \
		/* class elements */ \
		x(operator, NONE) D \
		/* member modifiers */ \
		x(public, FILE_ELEMENT_BEGIN) D \
		x(private, FILE_ELEMENT_BEGIN) D \
		x(override, FILE_ELEMENT_BEGIN) D \
		x(static, FILE_ELEMENT_BEGIN) D \
		x(mut, FILE_ELEMENT_BEGIN) D \
		x(const, FILE_ELEMENT_BEGIN | STMT_BEGIN) D \
		x(constexpr, FILE_ELEMENT_BEGIN | STMT_BEGIN) D \
		x(pure, FILE_ELEMENT_BEGIN) D \
		/* statements */ \
        x(if, STMT_BEGIN) D \
		x(else, NONE) D \
        x(while, STMT_BEGIN) D \
		x(for, STMT_BEGIN) D \
		x(in, NONE) D \
        x(return, STMT_BEGIN) D \
		/* expressions */ \
		x(this, EXPR_BEGIN) D \
		x(_, EXPR_BEGIN) D \
		/* named types */ \
        x(void, IS_TYPE) D \
		x(bool, IS_TYPE) D \
		x(int, IS_TYPE) D \
		x(float, IS_TYPE) D \
		x(auto, IS_TYPE) D \
		/* signed int types */ \
		x(i8, IS_TYPE) D \
		x(i16, IS_TYPE) D \
		x(i32, IS_TYPE) D \
		x(i64, IS_TYPE) D \
		/* unsigned int types */ \
		x(u8, IS_TYPE) D \
		x(u16, IS_TYPE) D \
		x(u32, IS_TYPE) D \
		x(u64, IS_TYPE) D \
		/* float types */ \
		x(f32, IS_TYPE) D \
		x(f64, IS_TYPE) D \
		/* class elements */ \
		x(volatile, NONE)

#define KEYWORD_STRING(m_kw, m_flags) #m_kw
#define KEYWORD_SET_FLAG(m_kw, m_flags) [#m_kw] = m_flags

namespace {

    // Simple list of keywords
    const std::unordered_set<std::string> keywords = {
		KEYWORDS(KEYWORD_STRING, COMMA)
    };

	std::map<std::string, cmp::TokenFlagBits> value_flags;

	void init_flags() {
		using enum cmp::TokenFlagBits;
		KEYWORDS(value_flags KEYWORD_SET_FLAG, ;);
	}

    // Determines the type of token
    cmp::TokenType get_token_type(const std::string& token) {
        static std::regex identifier(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");
        static std::regex int_number(R"(^[+-]?\d+$)");
        static std::regex double_number(R"(^[+-]?\d+\.\d+$)");
        static std::regex string(R"(^\"(.*)\"$)");
        static std::regex special(R"(^[{}()\[\]:;.,=+\-*/<>!&|@%]+$)");

        if (keywords.count(token)) return cmp::TokenType::KEYWORD;
        if (std::regex_match(token, double_number)) return cmp::TokenType::DOUBLE_LITERAL;
        if (std::regex_match(token, int_number)) return cmp::TokenType::INT_LITERAL;
        if (std::regex_match(token, string)) return cmp::TokenType::STRING_LITERAL;
        if (std::regex_match(token, identifier)) return cmp::TokenType::IDENTIFIER;
        if (std::regex_match(token, special)) return cmp::TokenType::SPECIAL;

        return cmp::TokenType::SPECIAL;
    }

} // anonymous namespace

namespace cmp {

std::vector<Token> tokenize(const std::string& source) {
	if (value_flags.empty()) init_flags();

	std::vector<Token> tokens;

    // Regex parts:
    const std::string single_line_comment = R"(//[^\n]*)";
    const std::string multi_line_comment = R"(/\*[\s\S]*?\*/)";
    const std::string whitespace = R"(\s+)";
    const std::string identifier = R"([a-zA-Z_][a-zA-Z0-9_]*)";
    const std::string annotation = R"(@[a-zA-Z_][a-zA-Z0-9_]*)";
    const std::string macro_start = R"([a-zA-Z_][a-zA-Z0-9_]*!\()";
    const std::string double_number = R"([+-]?\d+\.\d+)";
    const std::string int_number = R"([+-]?\d+)";
    // Multi-char operators first, then single-char
    const std::string special = R"(>=|<=|==|!=|\+=|-=|\*=|/=|[{}()\[\]:,=+\-*/%<>.])";
    const std::string string_literal = R"(\"(.*)\")";
    const std::string char_literal = R"('(\\.|[^'\\])')";

    // Master regex: all pieces combined
    std::regex token_pattern(
        single_line_comment + "|" +
        multi_line_comment + "|" +
        whitespace + "|" +
        string_literal + "|" +
        char_literal + "|" +
        identifier + "|" +
        double_number + "|" +
        int_number + "|" +
        special,
        std::regex::ECMAScript
    );

    auto begin = std::sregex_iterator(source.begin(), source.end(), token_pattern);
    auto end = std::sregex_iterator();

	bool is_newline = true;

	size_t current_pos = 0;
	int line = 1;
	int col = 1;

    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;

		std::string token_text = match.str();
		size_t start_pos = match.position();

		for (size_t j = current_pos; j < start_pos; ++j) {
			if (source[j] == '\n') {
				++line;
				col = 1;
			} else {
				++col;
			}
		}
		current_pos = start_pos;

		if (token_text.find('\n') != std::string::npos) {
			is_newline = true;
		}

        // Skip whitespace and comments
        if (std::regex_match(token_text, std::regex(whitespace)) ||
            std::regex_match(token_text, std::regex(single_line_comment)) ||
            std::regex_match(token_text, std::regex(multi_line_comment))) {
            continue;
        }

        TokenType type = get_token_type(token_text);

		TokenFlagBits flags = TokenFlagBits::NONE;

		switch (type)
		{
			case TokenType::KEYWORD:
				flags = value_flags[token_text];
				break;
			case TokenType::INT_LITERAL:
				flags = (
					TokenFlagBits::IS_EXPR
				);
				break;
			case TokenType::DOUBLE_LITERAL:
				flags = (
					TokenFlagBits::IS_EXPR
				);
				break;
			case TokenType::STRING_LITERAL:
				flags = (
					TokenFlagBits::IS_EXPR
				);
				break;
			case TokenType::REGEX_LITERAL:
				flags = (
					TokenFlagBits::IS_EXPR
				);
				break;
			case TokenType::IDENTIFIER:
				flags = (
					TokenFlagBits::IS_EXPR
					| TokenFlagBits::IS_TYPE
				);
				break;
			case TokenType::SPECIAL:
				// TODO
				break;
			case TokenType::ANNOTATION:
				flags = (
					TokenFlagBits::IS_EXPR
					| TokenFlagBits::IS_TYPE
				);
				break;
			case TokenType::MACRO_START:
				flags = (
					TokenFlagBits::IS_EXPR
					| TokenFlagBits::IS_TYPE
				);
				break;
		}

		if (type == TokenType::KEYWORD) {
			flags = value_flags[token_text];
		}

        tokens.emplace_back(
			type,
			token_text,
			flags,
			line,
			col,
			start_pos,
			is_newline
		);

		is_newline = false;
	}

	return tokens;
}

}