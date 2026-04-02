#pragma once
#include <string>

namespace cmp {

struct Error {
	enum Level {
		CRITICAL,
		ERROR,
		WARNING,
		HINT
	};

    const size_t start;
	const size_t end;
    const std::string message;
	const Level level;

    Error(size_t start, size_t end, std::string message, Level level)
			: start(start), end(end), message(message), level(level) {
		if (level == CRITICAL) {
			abort();
		}
	}
};

}