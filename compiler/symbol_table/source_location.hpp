#pragma once

#include "source_file.hpp"
#include "compiler_error.hpp"

namespace cmp {

struct SourceLocation {
	const SourceFile* source_file = nullptr;
	size_t start_token_index = 0;
	size_t end_token_index = 0;

	SourceLocation() = default;

	SourceLocation(
		const SourceFile* source_file,
		size_t start_token_index,
		size_t end_token_index)
			:	source_file(source_file),
				start_token_index(start_token_index),
				end_token_index(end_token_index) {
					COMPILER_ASSERT(start_token_index <= end_token_index, "start token index must not be larger than end token index");
				}

	std::string get_text() const;
	std::string get_text_as_rect() const;
};

} // namespace cmp
