#pragma once

#include <string>
#include <vector>
#include "token.hpp"

namespace cmp {

std::vector<Token> tokenize(const std::string& source);

}