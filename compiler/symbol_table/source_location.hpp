#pragma once

#include "source_file.hpp"

namespace cmp {

struct SourceLocation {
	const SourceFile& source_file;
	const size_t start_token_index;
	const size_t end_token_index;

	SourceLocation(
		const SourceFile& source_file,
		const size_t start_token_index,
		const size_t end_token_index)
			:	source_file(source_file),
				start_token_index(start_token_index),
				end_token_index(end_token_index) {}

	std::string get_text();
	std::string get_text_as_rect();
};

} // namespace cmp
