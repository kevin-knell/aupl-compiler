#pragma once
#include <string>

namespace cmp {

struct Error {
    int pos;
    std::string message;

    Error(int pos, std::string message) : pos(pos), message(message) {}
};

}