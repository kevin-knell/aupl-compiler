#pragma once
#include <string>

#include "text_color.hpp"

namespace cmp {

struct Error {
	enum Level {
		HINT,
		WARNING,
		ERROR,
		CRITICAL,
	};

	const std::string file_path;
    const size_t start;
	const size_t end;
	const int line;
	const int col;
    const std::string message;
	const Level level;

    Error(
			std::string file_path,
			size_t start,
			size_t end,
			int line,
			int col,
			std::string message,
			Level level)
				:	file_path(file_path),
					start(start),
					end(end),
					line(line),
					col(col),
					message(message),
					level(level) {}

	std::string get_error_text() const {
		switch (level) {
			case HINT:		return C_HINT("hint: ");
			case WARNING:	return C_WARNING("warning: ");
			case ERROR:		return C_ERROR("error: ");
			case CRITICAL:	return C_CRITICAL("critical: ");
			default: throw std::runtime_error("invalid error level: " + static_cast<int>(level));
		}
	}
};

}