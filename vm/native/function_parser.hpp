#pragma once

#include <string>
#include <vector>


struct FunctionSignature {
    std::string return_type;
    std::vector<std::string> parameter_types;
    std::vector<std::string> parameters;
    bool is_const;
};

class FunctionParser {
public:
    static FunctionSignature parse(const std::string& code);
};