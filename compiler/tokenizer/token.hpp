#pragma once
#include <string>

namespace cmp {

enum class TokenType {
    IDENTIFIER,
    KEYWORD,
    INT_LITERAL,
    DOUBLE_LITERAL,
    STRING_LITERAL,
    REGEX_LITERAL,
    SPECIAL
};

struct Token {
    TokenType type;
    std::string value;
    int pos;
    bool is_new_line;

    Token(TokenType type, std::string value, int pos, int is_new_line)
        : type(type), value(value), pos(pos), is_new_line(is_new_line) {}
};

}