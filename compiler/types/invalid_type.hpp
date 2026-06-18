#pragma once
#include "type.hpp"
#include "text_color.hpp"

namespace cmp
{

struct InvalidType final : public Type {
    std::string name;

    static TypePtr make(const std::string& n) {
        return std::make_shared<InvalidType>(n);
    }

    InvalidType(const std::string& n) : name(n) {}

    std::string to_string() const override {
		return "(invalid)" + C_ERROR(name);
	}

    size_t get_size() const override { return 16; };

	KIND get_kind() const override { return INVALID; };

	bool is_cpp_type(const std::string&) const override { return false; }

	bool is_convertable_to_cpp_type(const std::string&) const override { return false; }
};

} // namespace cmp
