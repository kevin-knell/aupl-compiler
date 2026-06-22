#pragma once
#include <string>
#include <cstdint>

namespace cmp {

enum class TokenType {
	IDENTIFIER,
	KEYWORD,
	INT_LITERAL,
	DOUBLE_LITERAL,
	STRING_LITERAL,
	REGEX_LITERAL,
	SPECIAL,
	ANNOTATION,
	MACRO_START,
};

enum class TokenFlagBits : unsigned {
	NONE =					0,

	FILE_ELEMENT_BEGIN =	1 << 0,
	STMT_BEGIN =			1 << 1,
	EXPR_BEGIN =			1 << 2 | STMT_BEGIN,
	TYPE_BEGIN =			1 << 3,
	CLOSING_PAREN =			1 << 4,

	IS_EXPR =				EXPR_BEGIN,
	IS_TYPE =				FILE_ELEMENT_BEGIN | STMT_BEGIN | EXPR_BEGIN | TYPE_BEGIN
};

constexpr TokenFlagBits operator|(TokenFlagBits left, TokenFlagBits right) {
	return static_cast<TokenFlagBits>(
		static_cast<uint32_t>(left)
		| static_cast<uint32_t>(right)
	);
}

constexpr TokenFlagBits operator&(TokenFlagBits left, TokenFlagBits right) {
	return static_cast<TokenFlagBits>(
		static_cast<uint32_t>(left)
		& static_cast<uint32_t>(right)
	);
}

constexpr TokenFlagBits operator~(TokenFlagBits bits) {
	return static_cast<TokenFlagBits>(
		~static_cast<uint32_t>(bits)
	);
}

struct Token {
    TokenType type;
    std::string value;
	TokenFlagBits flags;
	int line;
	int col;
    size_t pos;
    bool is_new_line;

    Token(
		TokenType type,
		std::string value,
		TokenFlagBits flags,
		int line,
		int col,
		size_t pos,
		bool is_new_line)
			:	type(type),
				value(value),
				flags(flags),
				line(line),
				col(col),
				pos(pos),
				is_new_line(is_new_line) {}
	
	bool has_flag(TokenFlagBits flag) const {
		return (flag & flags) != TokenFlagBits::NONE;
	}
};

}