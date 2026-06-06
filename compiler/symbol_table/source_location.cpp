#include "source_location.hpp"

#include <sstream>
#include <iostream>
#include <format>
#include <algorithm>
#include <numeric>
#include "compiler_error.hpp"

constexpr size_t MAX_TABS = 256;

namespace {
	std::vector<std::string> get_lines(std::string text) {
		std::vector<std::string> result;
		std::stringstream ss;
		
		for (char c : text) {
			if (c == '\n') {
				result.push_back(ss.str());
				ss = std::stringstream();
			} else {
				ss << c;
			}
		}

		result.push_back(ss.str());

		return result;
	}

	size_t line_get_tabs(std::string line) {
		size_t result = 0;

		for (char c : line) {
			if (c != '\t') {
				break;
			}

			++result;
		}

		return result;
	}

	std::string make_line_with_number(size_t number, std::string line) {
		return "\t" + std::format("{:>4}| ", number) + line;
	}
} // namespace


std::string cmp::SourceLocation::get_text() const {
	if (!source_file) return "";

	const Token start_token = source_file->tokens[start_token_index];
	const Token end_token = source_file->tokens[end_token_index - 1];

	return source_file->text.substr(start_token.pos, end_token.pos + end_token.value.size() - start_token.pos);
}

std::string cmp::SourceLocation::get_text_as_rect() const {
	if (!source_file) return "";
	
	const std::string text = get_text();

	const std::vector<std::string> lines = get_lines(text);

	const size_t line_number_start = source_file->tokens[start_token_index].line;

	if (lines.size() == 1) return make_line_with_number(line_number_start, lines.front());

	size_t min_tabs = MAX_TABS;

	for (auto it = lines.begin() + 1; it != lines.end(); ++it) {
		if (it->empty()) continue;
		size_t current_line_tabs_amount = line_get_tabs(*it);
		min_tabs = std::min(min_tabs, current_line_tabs_amount);
	}

	std::stringstream result;
	size_t current_line_number = line_number_start;

	std::cout << "size: " << lines.size() << std::endl;

	for (auto it = lines.begin(); it != lines.end(); ++it) {
		std::string line;

		if (it != lines.begin() && it->size() > min_tabs) {
			line = it->substr(min_tabs);
		} else {
			line = *it;
		}

		result << make_line_with_number(current_line_number++, line);
		result << '\n';
	}
	
	return result.str();
}
