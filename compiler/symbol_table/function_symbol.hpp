#pragma once

#include "type.hpp"
#include "variable_symbol.hpp"
#include "scope.hpp"
#include "bytecode_generation_info.hpp"
#include <string>
#include <vector>
#include <map>

namespace cmp {

struct FunctionSymbol {
	TypePtr return_type;
	std::string name;
	std::vector<VarPtr> parameters;
	ScopePtr scope;

	bool is_public = false;
	bool is_static = false;
	bool is_const = false;
	bool is_abstract = false;
	bool is_pure = false;

	// Default constructor
	FunctionSymbol() = default;

	// Custom constructor
	FunctionSymbol(const TypePtr& return_type,
		const std::string& name,
		const std::vector<VarPtr>& parameters,
		const ScopePtr& scope)
		: return_type(std::move(return_type)), name(name), parameters(parameters), scope(std::move(scope)) {}

	std::string head_to_string();
	std::string to_string();

    size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const;
};

using FuncPtr = std::shared_ptr<FunctionSymbol>;

}