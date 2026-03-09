#include "function_parser.hpp"
#include <regex>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace {
    std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t\n\r");
        auto end   = s.find_last_not_of(" \t\n\r");
        if (start == std::string::npos || end == std::string::npos) return "";
        return s.substr(start, end - start + 1);
    }

    void splitParams(const std::string& params, std::vector<std::string>& types, std::vector<std::string>& names) {
        std::stringstream ss(params);
        std::string param;

		std::regex pattern(R"(^((const\s+)?\w+&?)\s+(\w+)$)");

        while (std::getline(ss, param, ',')) {
            param = trim(param);
            if (param.empty()) continue;
			//std::cout << "param: " << param << std::endl;

			std::smatch match;
			if (!std::regex_search(param, match, pattern)) continue;

			types.push_back(match[1]);
			names.push_back(match[3]);
        }
    }
}

FunctionSignature FunctionParser::parse(const std::string& code) {
    FunctionSignature sig;
    static const std::regex pattern(
        R"(^(\w+)\s*(\(\w+::\*\)|\(\*\))?\(([\w\s,&*]*)\)\s*(const)?$)"
    );

    std::smatch match;
    if (!std::regex_search(code, match, pattern)) return sig;

    std::string rawReturn = match[1].str();
    std::string obj = match[2].str();
    std::string rawParams = match[3].str();

    sig.return_type = rawReturn;

    sig.is_const   = match[4].matched;
    splitParams(rawParams, sig.parameter_types, sig.parameters);

    return sig;
}
