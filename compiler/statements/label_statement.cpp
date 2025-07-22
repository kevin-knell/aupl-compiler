#include "label_statement.hpp"

#include <iostream>

namespace cmp {

std::vector<uint8_t> LabelStatement::generate_bytecode(BytecodeGenerationInfo &bgi) const {
    return {};
}

size_t LabelStatement::get_bytecode_size(BytecodeGenerationInfo &bgi) const {
	if (bgi.scope->label_addresses.find(identifier) != bgi.scope->label_addresses.end()) {
		std::cout << "ERROR, already has label " << to_string() << ": " << bgi.scope->label_addresses[identifier] << std::endl;
	}
    bgi.scope->label_addresses[identifier] = bgi.bytecode_size;
    return size_t();
}

Statement::KIND LabelStatement::get_kind() const {
    return LABEL;
}

std::vector<ExprPtr *> LabelStatement::get_expressions() {
    return std::vector<ExprPtr *>();
}

std::string LabelStatement::to_string() const {
    return "@" + identifier;
}

}