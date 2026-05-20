#pragma once

#include "variable_symbol.hpp"
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include "forward_declarations.hpp"

namespace cmp {

class LabelStatement;

struct Scope : public std::enable_shared_from_this<Scope> {
private:
	struct Private{ explicit Private() = default; };

public:
    enum SCOPE_TYPE {
		GLOBAL,
        STATIC_CLASS,
        CLASS,
        FUNCTION,
        FUNCTION_SUB
    };

    std::string name;
    const SCOPE_TYPE type;
    std::weak_ptr<Scope> upper_scope;
    std::vector<std::weak_ptr<Scope>> lower_scopes;
    std::vector<StmtPtr> body;
    size_t starting_address;
    std::map<std::string, size_t> label_addresses;
    std::map<std::string, VarPtr> variables;
    std::map<std::string, size_t> variable_indices;
	std::vector<std::string> args;
	
    size_t size = 0;
    size_t temp_count = 0;
	mutable int label_count = 0;

	static ScopePtr create(SCOPE_TYPE type, const std::string& name) {
		return std::make_shared<Scope>(Private(), type, name);
	}

    static ScopePtr find_scope(const ScopePtr& scope, const std::string& name);
	static size_t get_variable_index(const ScopePtr& scope, const std::string& name);

    Scope(Private, SCOPE_TYPE type, const std::string& name) : name(name), type(type) {};
    
	void generate_structure(size_t offset = 0);
    bool has(std::string name);
    std::string get_full_name();
    std::string structure_to_string() const;
    VarPtr get_temp(TypePtr type, ExprPtr init_val, std::string temp_name = "tmp");
	std::string get_label_name(std::string name) const;
};

}