// tokenizer.cpp
#include "tokenizer.hpp"
#include <regex>
#include <unordered_set>
#include <string>
#include <vector>

namespace {

    // Simple list of keywords
    const std::unordered_set<std::string> keywords = {
		// header
        "class", "interface", "singleton",
		// class-like
		"struct", "impl",
		// member infc
		"public", "private",
		"static",
		"const",
		// statements
        "if", "elif", "else",
        "while", "for", "in",
        "return",
		"_", "valof",
		// epxressions
		"this",
		"constexpr", "is_constexpr",
		// types
        "void", "bool", "int", "float",
		"auto",
		"sizeof", "typeof",
		"tupletypes", "typecount",
		// compiler-related
		"volatile", "compileflag"
    };

    // Determines the type of token
    cmp::TokenType get_token_type(const std::string& token) {
        static std::regex identifier(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");
        static std::regex int_number(R"(^[+-]?\d+$)");
        static std::regex double_number(R"(^[+-]?\d+\.\d+$)");
        static std::regex string(R"(^\"(.*)\"$)");
        static std::regex special(R"(^[{}()\[\]:;.,=+\-*/<>!&|]$@)");

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
    std::vector<Token> tokens;

    // Regex parts:
    const std::string single_line_comment = R"(//[^\n]*)";
    const std::string multi_line_comment = R"(/\*[\s\S]*?\*/)";
    const std::string whitespace = R"(\s+)";
    const std::string identifier = R"([a-zA-Z_][a-zA-Z0-9_]*)";
    const std::string double_number = R"([+-]?\d+\.\d+)";
    const std::string int_number = R"([+-]?\d+)";
    const std::string special = R"([{}()\[\]:;,=+\-*/<>!&|.@])";
    const std::string string_literal = R"(^\"(.*)\"$)";
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

    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        std::string token_text = match.str();
        size_t start_pos = match.position();

        // Skip whitespace and comments
        if (std::regex_match(token_text, std::regex(whitespace)) ||
            std::regex_match(token_text, std::regex(single_line_comment)) ||
            std::regex_match(token_text, std::regex(multi_line_comment))) {
            continue;
        }

        TokenType type = get_token_type(token_text);
        tokens.push_back(Token(type, token_text, start_pos, false));
    }

    return tokens;
}

}