#pragma once

#include <cstdint>

#include "native.hpp"

namespace auplib
{

class Time {
public:
	static void register_to_db(vm::ClassDB& db);

	DECLARE_UTILITY_CLASS(Time)

	static uint64_t sys_msec();
	static uint64_t sys_usec();
};

} // namespace auplib
