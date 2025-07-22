#pragma once
#include <memory>
#include <string>
#include <iostream>
#include "scope.hpp"
#include "label_statement.hpp"

namespace cmp {

struct Label {
	std::shared_ptr<Scope> scope;
	std::shared_ptr<LabelStatement> label_stmt; // optional, can be nullptr
	std::string identifier;

	Label(const std::shared_ptr<Scope>& scope_, const std::string& id, std::shared_ptr<LabelStatement> label_stmt_ = nullptr)
		: scope(scope_), label_stmt(label_stmt_), identifier(id) {}

	size_t get_address() {
		if (label_stmt) {
			const std::string& key = label_stmt->identifier;
			if (scope->label_addresses.find(key) == scope->label_addresses.end()) {
				std::cout << "ERROR" << std::endl;
				return 0;
			} else {
				return scope->label_addresses[key];
			}
		} else {
			return scope->starting_address;
		}
	}

	std::string to_string() const {
		return label_stmt ? label_stmt->to_string() : ("@" + identifier);
	}
};

} // namespace cmp
