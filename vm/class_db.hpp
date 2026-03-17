#pragma once

#include <vector>
#include <iostream>
#include <regex>

#include "function_parser.hpp"
#include "type_traits.hpp"
#include "method_bind.hpp"

namespace vm {

struct MethodPair {
    std::string name;
	std::string signature;
    MethodFunc value_call;
    MethodFunc pointer_call;
	bool is_global = false;
	std::vector<std::string> arg_types;
	size_t arg_count;
	uint16_t class_id;
	uint16_t method_id;
	bool is_constructor;
};

struct ClassBind {
    const std::string name;
    const int id;
	const size_t size;
    std::vector<MethodPair> static_methods;
    std::vector<MethodPair> methods;

	ClassBind(const std::string& name, size_t id, size_t size)
		: name(name), id(id), size(size) {}
};

namespace {
	std::string replace_member_pointer_with_name(const std::string& input, const std::string& funcName) {
    	return std::regex_replace(input, std::regex(R"(\(\w+::\*\)|\(\*\))"), funcName);
	}
}

class ClassDB {
public:
    std::vector<ClassBind> classes;

	template<typename ClassType>
    size_t register_class(std::string name) {
        classes.push_back(ClassBind(name, classes.size(), sizeof(ClassType)));
		return classes.back().id;
    }

    template<typename Constructor>
    void register_constructor(int class_id, std::string signature) {
        MethodPair pair;
		pair.name = classes[class_id].name;
		pair.signature = signature;
		pair.is_global = true;
        pair.value_call = bind_constructor<Constructor>();
        pair.pointer_call = bind_constructor<Constructor>();

		auto parsed = FunctionParser::parse(signature);
		pair.arg_types = parsed.parameter_types;
		pair.arg_count = parsed.parameters.size();

		pair.class_id = class_id;
		pair.method_id = classes[class_id].methods.size();

		pair.is_global = false;
		pair.is_constructor = true;

        classes[class_id].methods.push_back(std::move(pair));
    }

    template<typename Method>
    void register_method(int class_id, std::string name, Method method, std::string signature) {
        MethodPair pair;
        pair.name = name;
		pair.signature = replace_member_pointer_with_name(signature, name);
        pair.value_call = bind_method(method);
        pair.pointer_call = bind_method(method); //bind_method_ptr_args(method);

		auto parsed = FunctionParser::parse(signature);
		pair.arg_types = parsed.parameter_types;
		pair.arg_count = parsed.parameters.size();

		pair.class_id = class_id;
		pair.method_id = classes[class_id].methods.size();

		pair.is_global = false;
		pair.is_constructor = false;

		classes[class_id].methods.push_back(std::move(pair));
    }

    template<typename Method>
    void register_static_method(int class_id, std::string name, Method method, std::string signature, bool is_global = false) {
        MethodPair pair;
        pair.name = name;
		pair.signature = replace_member_pointer_with_name(signature, name);
        pair.value_call = bind_static_method(method);
        pair.pointer_call = bind_static_method(method); //bind_method_ptr_args(method);

		auto parsed = FunctionParser::parse(signature);
		pair.arg_types = parsed.parameter_types;
		pair.arg_count = parsed.parameters.size();

		pair.class_id = class_id;
		pair.method_id = classes[class_id].methods.size();

		pair.is_global = is_global;
		pair.is_constructor = false;

		classes[class_id].methods.push_back(std::move(pair));
    }
};

} // namespace vm
