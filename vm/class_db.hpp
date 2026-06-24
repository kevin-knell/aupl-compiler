#pragma once

#include <vector>
#include <iostream>
#include <regex>
#include <numeric>
#include <cassert>

#include "function_parser.hpp"
#include "type_traits.hpp"
#include "method_bind.hpp"
#include "native.hpp"
#include "object.hpp"

namespace vm {

typedef uint16_t ClassID_t;

struct VariableBind {
	std::string name;
	std::string type;
	ClassID_t class_id;
	ClassID_t var_id;
	ClassID_t setter_id;
	ClassID_t getter_id;
};

struct MethodPair {
    std::string name;
	std::string signature;
    MethodFunc value_call;
    MethodFunc pointer_call;
	bool is_global = false;
	std::string return_type;
	std::vector<std::string> arg_types;
	std::vector<std::string> arg_names;
	size_t arg_count;
	ClassID_t class_id;
	ClassID_t method_id;
	bool is_constructor;
};

struct ClassBind {
	// meta
    const std::string name;
	const std::vector<std::string> type_names;
    const ClassID_t id;
    ClassID_t parent_id;
	const size_t size;
	const bool is_object;
	const bool is_trivial;
	
	// static
	std::vector<VariableBind> static_variables;
    std::vector<MethodPair> static_methods;

	// local
	std::vector<VariableBind> variables;
    std::vector<MethodPair> methods;

	ClassBind(const std::string& name, std::vector<std::string> type_names, ClassID_t id, size_t size, bool is_object, bool is_trivial)
		: name(name), type_names(type_names), id(id), parent_id(id), size(size), is_object(is_object), is_trivial(is_trivial) {}
};

namespace {
	std::string replace_member_pointer_with_name(const std::string& input, const std::string& func_name) {
    	return std::regex_replace(input, std::regex(R"(\(\w+(<\w+>)?::\*\)|\(\*\))"), func_name);
	}

	std::vector<std::string> ExtractTypeNames(const std::string& s) {
		std::vector<std::string> result;
		std::string token;
		int depth = 0;

		auto flush_token = [&]() {
			if (!token.empty()) {
				result.push_back(token);
				token.clear();
			}
		};

		for (char c : s) {
			if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
				token += c;
			}
			else if (c == '<') {
				flush_token();
				++depth;
			}
			else if (c == '>') {
				flush_token();
				--depth;
			}
			else if (c == ',') {
				flush_token();
			}
			else {
				flush_token();
			}
		}

		flush_token();
		return result;
	}
}

class ClassDB {
public:
    std::vector<ClassBind> classes;

	template<typename ClassType>
    ClassID_t register_class(std::string name) {
		using auplib::Object;
		//std::cout << name << IS_OBJECT(ClassType) << std::endl;

		std::vector<std::string> names = ExtractTypeNames(name);

		std::string class_name = names.front();
		std::vector<std::string> type_names(names.size() - 1);
		std::copy(names.begin() + 1, names.end(), type_names.begin());

        classes.emplace_back(
			class_name,
			type_names,
			static_cast<ClassID_t>(classes.size()),
			sizeof(ClassType),
			IS_OBJECT(ClassType),
			IS_TRIVIAL(ClassType)
		);
		return classes.back().id;
    }

	template<typename ClassType, typename ParentType>
    ClassID_t register_class(std::string name, std::string parent_name) {
		static_assert(std::is_base_of_v<ParentType, ClassType>);

		ClassID_t id = register_class<ClassType>(name);
		
		ClassID_t parent_id = std::numeric_limits<ClassID_t>::max();

		for (size_t i = 0; i < classes.size(); ++i) {
			if (classes[i].name == parent_name) {
				parent_id = static_cast<ClassID_t>(i);
			}
		}

		assert(parent_id < std::numeric_limits<ClassID_t>::max());
		
		classes[id].parent_id = parent_id;

		return classes.back().id;
    }

    template<typename Constructor>
    ClassID_t register_constructor(ClassID_t class_id, std::string signature) {
        MethodPair pair;
		pair.name = classes[class_id].name;
		pair.signature = signature;
		pair.is_global = true;
        pair.value_call = bind_constructor<Constructor>();
        pair.pointer_call = bind_constructor<Constructor>();

		auto parsed = FunctionParser::parse(signature);
		pair.return_type = parsed.return_type;
		pair.arg_types = parsed.parameter_types;
		pair.arg_names = parsed.parameters;
		pair.arg_count = parsed.parameters.size();

		pair.class_id = class_id;
		pair.method_id = static_cast<ClassID_t>(classes[class_id].methods.size());

		pair.is_global = false;
		pair.is_constructor = true;

        classes[class_id].methods.push_back(std::move(pair));

		return pair.method_id;
    }

    template<typename Method>
    ClassID_t register_method(ClassID_t class_id, std::string name, Method method, std::string signature) {
        MethodPair pair;
        pair.name = name;
		pair.signature = replace_member_pointer_with_name(signature, name);
        pair.value_call = bind_method(method);
        pair.pointer_call = bind_method(method); //bind_method_ptr_args(method);

		auto parsed = FunctionParser::parse(signature);
		pair.return_type = parsed.return_type;
		pair.arg_types = parsed.parameter_types;
		pair.arg_names = parsed.parameters;
		pair.arg_count = parsed.parameters.size();

		pair.class_id = class_id;
		pair.method_id = static_cast<ClassID_t>(classes[class_id].methods.size());

		pair.is_global = false;
		pair.is_constructor = false;

		classes[class_id].methods.push_back(std::move(pair));

		return pair.method_id;
    }

    template<typename Method>
    ClassID_t register_static_method(ClassID_t class_id, std::string name, Method method, std::string signature, bool is_global = false) {
        MethodPair pair;
        pair.name = name;
		pair.signature = replace_member_pointer_with_name(signature, name);
        pair.value_call = bind_static_method(method);
        pair.pointer_call = bind_static_method(method); //bind_method_ptr_args(method);

		auto parsed = FunctionParser::parse(signature);
		pair.return_type = parsed.return_type;
		pair.arg_types = parsed.parameter_types;
		pair.arg_names = parsed.parameters;
		pair.arg_count = parsed.parameters.size();

		pair.class_id = class_id;
		pair.method_id = static_cast<ClassID_t>(classes[class_id].methods.size());

		pair.is_global = is_global;
		pair.is_constructor = false;

		classes[class_id].methods.push_back(std::move(pair));

		return pair.method_id;
    }

	template<typename T>
	ClassID_t register_variable(ClassID_t class_id, std::string name, std::string type, ClassID_t setter_id, ClassID_t getter_id) {
		ClassID_t var_id = static_cast<ClassID_t>(classes[class_id].variables.size());
		
		classes[class_id].variables.emplace_back(
			VariableBind{
				.name = name,
				.type = type,

				.class_id = class_id,
				.var_id = var_id,

				.setter_id = setter_id,
				.getter_id = getter_id
			}
		);

		return var_id;
	}
};

} // namespace vm
