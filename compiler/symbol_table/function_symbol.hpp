#pragma once

#include "type.hpp"
#include "variable_symbol.hpp"
#include "scope.hpp"
#include <string>
#include <vector>
#include <map>

namespace vm
{

class MethodPair;

} // namespace vm


namespace cmp {

struct FunctionSymbol final : public std::enable_shared_from_this<FunctionSymbol> {
private:
	struct Private{ explicit Private() = default; };

public:
	static FuncPtr create(vm::MethodPair& method_pair);

	static FuncPtr create(
		const TypePtr& return_type,
		const std::string& name,
		const std::vector<VarPtr>& parameters,
		const ScopePtr& scope,
		const bool is_constructor);

public:
	FunctionSymbol(
		Private,
		const TypePtr& return_type,
		const std::string& name,
		const std::vector<VarPtr>& parameters,
		const ScopePtr& scope,
		const bool is_constructor);
	
	FunctionSymbol(Private, vm::MethodPair& method_pair);
public:
	TypePtr return_type;
	std::string name;
	std::vector<VarPtr> parameters;
	ScopePtr scope;
	vm::MethodPair* method_pair = nullptr;
	bool is_constructor = false;

	bool is_public = false;
	bool is_static = false;
	bool is_const = false;
	bool is_abstract = false;
	bool is_pure = false;
	
	FuncPtr shared_ptr() {
		return shared_from_this();
	}

	std::string head_to_string();
	std::string to_string();
	std::string to_cpp_string_prototype();
	std::string to_cpp_string(std::string context);
};

using FuncPtr = std::shared_ptr<FunctionSymbol>;

}