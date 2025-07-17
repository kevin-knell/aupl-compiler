#pragma once
#include <memory>
#include <string>
#include "scope.hpp"
#include "label_statement.hpp"

namespace cmp {

struct Label {
    std::shared_ptr<Scope> scope;
    std::shared_ptr<LabelStatement> label_stmt; // optional, can be nullptr
    std::string identifier;

    Label(const std::shared_ptr<Scope>& scope_, const std::string& id, std::shared_ptr<LabelStatement> label_stmt_ = nullptr)
        : scope(scope_), label_stmt(label_stmt_), identifier(id) {}

	std::string to_string() const {
		return label_stmt ? label_stmt->to_string() : ("@" + identifier);
	}
};

} // namespace cmp
