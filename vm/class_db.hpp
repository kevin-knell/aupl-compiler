#pragma once

#include <vector>
#include <iostream>
#include <regex>

#include "function_parser.hpp"
#include "type_traits.hpp"
#include "method_bind.hpp"
#include "native.hpp"

namespace vm {

struct VariableBind {
	std::string name;
	std::string type;
	uint16_t class_id;
	uint16_t var_id;
	uint16_t setter_id;
	uint16_t getter_id;
};

struct MethodPair {
    std::string name;
	std::string signature;
    MethodFunc value_call;
    MethodFunc pointer_call;
	bool is_global = false;
	std::vector<std::string> arg_types;
	std::vector<std::string> arg_names;
	size_t arg_count;
	uint16_t class_id;
	uint16_t method_id;
	bool is_constructor;
};

struct ClassBind {
	// meta
    const std::string name;
    const int16_t id;
	const size_t size;
	const bool is_object;
	const bool is_trivial;
	
	// static
	std::vector<VariableBind> static_variables;
    std::vector<MethodPair> static_methods;

	// local
	std::vector<VariableBind> variables;
    std::vector<MethodPair> methods;

	ClassBind(const std::string& name, int16_t id, size_t size, bool is_object, bool is_trivial)
		: name(name), id(id), size(size), is_object(is_object), is_trivial(is_trivial) {}
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
    int16_t register_class(std::string name) {
        classes.emplace_back(name, static_cast<int16_t>(classes.size()), sizeof(ClassType), IS_OBJECT(ClassType), IS_TRIVIAL(ClassType));
		return classes.back().id;
    }

    template<typename Constructor>
    void register_constructor(int16_t class_id, std::string signature) {
        MethodPair pair;
		pair.name = classes[class_id].name;
		pair.signature = signature;
		pair.is_global = true;
        pair.value_call = bind_constructor<Constructor>();
        pair.pointer_call = bind_constructor<Constructor>();

		auto parsed = FunctionParser::parse(signature);
		pair.arg_types = parsed.parameter_types;
		pair.arg_names = parsed.parameters;
		pair.arg_count = parsed.parameters.size();

		pair.class_id = class_id;
		pair.method_id = static_cast<int16_t>(classes[class_id].methods.size());

		pair.is_global = false;
		pair.is_constructor = true;

        classes[class_id].methods.push_back(std::move(pair));
    }

    template<typename Method>
    void register_method(int16_t class_id, std::string name, Method method, std::string signature) {
        MethodPair pair;
        pair.name = name;
		pair.signature = replace_member_pointer_with_name(signature, name);
        pair.value_call = bind_method(method);
        pair.pointer_call = bind_method(method); //bind_method_ptr_args(method);

		auto parsed = FunctionParser::parse(signature);
		pair.arg_types = parsed.parameter_types;
		pair.arg_names = parsed.parameters;
		pair.arg_count = parsed.parameters.size();

		pair.class_id = class_id;
		pair.method_id = static_cast<int16_t>(classes[class_id].methods.size());

		pair.is_global = false;
		pair.is_constructor = false;

		classes[class_id].methods.push_back(std::move(pair));
    }

    template<typename Method>
    void register_static_method(int16_t class_id, std::string name, Method method, std::string signature, bool is_global = false) {
        MethodPair pair;
        pair.name = name;
		pair.signature = replace_member_pointer_with_name(signature, name);
        pair.value_call = bind_static_method(method);
        pair.pointer_call = bind_static_method(method); //bind_method_ptr_args(method);

		auto parsed = FunctionParser::parse(signature);
		pair.arg_types = parsed.parameter_types;
		pair.arg_names = parsed.parameters;
		pair.arg_count = parsed.parameters.size();

		pair.class_id = class_id;
		pair.method_id = static_cast<int16_t>(classes[class_id].methods.size());

		pair.is_global = is_global;
		pair.is_constructor = false;

		classes[class_id].methods.push_back(std::move(pair));
    }

	template<typename T>
	void register_variable(uint16_t class_id, std::string name, std::string type) {
		classes[class_id].variables.emplace_back(
			VariableBind{
				.name = name,
				.type = type,

				.class_id = class_id,
				.var_id = static_cast<uint16_t>(classes[class_id].variables.size()),

				.setter_id = 0,
				.getter_id = 0
			}
		);
	}
};

} // namespace vm
