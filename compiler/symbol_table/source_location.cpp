#include "source_location.hpp"

#include <sstream>
#include <iostream>
#include <format>
#include <algorithm>
#include <numeric>
#include "compiler_error.hpp"

std::string cmp::SourceLocation::get_text() {
	const Token start_token = source_file.tokens[start_token_index];
	const Token end_token = source_file.tokens[end_token_index];

	return source_file.text.substr(start_token.pos, end_token.pos + end_token.value.size() - start_token.pos);
}

std::string cmp::SourceLocation::get_text_as_rect() {
	std::string text = get_text();

	std::vector<std::string> lines;
	std::vector<size_t> tabs_per_line;

	std::stringstream current_line;
	size_t current_tabs = 0;

	//current_line << "\t" << std::format("{:>4}| ", source_file.tokens[start_token_index].line);

	bool has_non_whitespace = false;
	size_t min_tabs = std::numeric_limits<size_t>::max();

	for (char c : text) {
		if (c == '\t') {
			current_tabs += 1;
		} else if (c == '\n') {

			if (has_non_whitespace) {
				if (!lines.empty()) {
					min_tabs = std::min(min_tabs, current_tabs);
				}
				tabs_per_line.push_back(current_tabs);
			} else {
				tabs_per_line.push_back(min_tabs);
			}

			lines.push_back(current_line.str());

			current_line = std::stringstream();
			current_tabs = 0;
			has_non_whitespace = false;
		} else {
			current_line << c;
			has_non_whitespace = true;
		}
	}

	COMPILER_ASSERT(min_tabs < std::numeric_limits<size_t>::max(), "");

	tabs_per_line[0] = min_tabs;

	size_t start_line = source_file.tokens[start_token_index].line;

	std::stringstream result;

	for (size_t line_idx = 0; line_idx < lines.size(); ++line_idx) {
		result
			<< '\t'
			<< std::format("{:>4}| ", start_line + line_idx)
			<< '\t';
		
		const std::string& line = lines[line_idx];
		for (size_t tab_idx = min_tabs; tab_idx < tabs_per_line[line_idx]; ++tab_idx) {
			result << '\t';
		}
		
		if (line_idx > 0 && line.size() > min_tabs) {
			result << line.substr(min_tabs);
		} else {
			result << line;
		}
		
		result << '\n';
	}
	
	return result.str();
}
