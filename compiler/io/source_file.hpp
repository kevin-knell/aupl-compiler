#pragma once

#include <string>
#include <vector>
#include "token.hpp"

namespace cmp {

struct SourceFile {
	const std::string path;
	const std::string text;
	const std::vector<Token> tokens;

	SourceFile() = delete;
	SourceFile(const SourceFile&) = delete;
	SourceFile& operator=(const SourceFile&) = delete;

	SourceFile(SourceFile&&) = default;
	SourceFile& operator=(SourceFile&&) = default;

	SourceFile(
		const std::string path,
		const std::string text,
		const std::vector<Token> tokens) : path(path), text(text), tokens(tokens) {}
};

} // namespace cmp
