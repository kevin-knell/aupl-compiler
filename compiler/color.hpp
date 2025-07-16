#pragma once
#include <string>

extern int paren_count;

#define CLEAR               std::string("\033[0m")
#define ITALIC              std::string("\033[3m")
#define FAINT               std::string("\033[2m")
#define STRIKETHROUGH       std::string("\033[9m")

#define RED                 std::string("\033[31m")
#define GREEN               std::string("\033[32m")
#define YELLOW              std::string("\033[33m")
#define BLUE                std::string("\033[34m")
#define MAGENTA             std::string("\033[35m")
#define CYAN                std::string("\033[36m")
#define GREY                std::string("\033[90m")

#define RED_BG              std::string("\033[41m")
#define GREEN_BG            std::string("\033[42m")
#define YELLOW_BG           std::string("\033[43m")
#define BLUE_BG             std::string("\033[44m")
#define MAGENTA_BG          std::string("\033[45m")
#define CYAN_BG             std::string("\033[46m")
#define GREY_BG             std::string("\033[100m")

const std::string PAREN_COLORS[4] = {
    YELLOW, GREEN, MAGENTA, BLUE
};

#define C_TYPE(s)           std::string(GREEN + ITALIC + s + CLEAR)
#define C_KEYWORD(s)        std::string(MAGENTA + ITALIC + s + CLEAR)
#define C_LITERAL(s)        std::string(CLEAR + s + CLEAR)
#define C_IDENTIFIER(s)     std::string(CYAN + s + CLEAR)
#define C_TEMP(s)           std::string(GREY + s + CLEAR)
#define C_HIDE(s)           std::string(FAINT + s + CLEAR)
#define C_ERROR(s)          std::string(RED + STRIKETHROUGH+ s + CLEAR)
#define C_UNRES(s)          std::string(RED_BG + s + CLEAR)

#define C_PAREN_L           std::string(PAREN_COLORS[paren_count-- % 4] + "(" + CLEAR)
#define C_BRACE_L           std::string(PAREN_COLORS[paren_count-- % 4] + "{" + CLEAR)
#define C_BRACKET_L         std::string(PAREN_COLORS[paren_count-- % 4] + "[" + CLEAR)
#define C_POINTY_L          std::string(PAREN_COLORS[paren_count-- % 4] + "<" + CLEAR)

#define C_PAREN_R           std::string(PAREN_COLORS[++paren_count % 4] + ")" + CLEAR)
#define C_BRACE_R           std::string(PAREN_COLORS[++paren_count % 4] + "}" + CLEAR)
#define C_BRACKET_R         std::string(PAREN_COLORS[++paren_count % 4] + "]" + CLEAR)
#define C_POINTY_R          std::string(PAREN_COLORS[++paren_count % 4] + ">" + CLEAR)

#define C_PAREN(s)          std::string(C_PAREN_L + s + C_PAREN_R)
#define C_BRACE(s)          std::string(C_BRACE_L + s + C_BRACE_R)
#define C_BRACKET(s)        std::string(C_BRACKET_L + s + C_BRACKET_R)
#define C_POINTY(s)         std::string(C_POINTY_L + s + C_POINTY_R)