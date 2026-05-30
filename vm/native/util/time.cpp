#include "time.hpp"

#include <chrono>
#include "class_db.hpp"

namespace auplib {

void Time::register_to_db(vm::ClassDB& db) {
	const uint16_t ID = REGISTER_CLASS(Time);

	REGISTER_STATIC_METHOD(ID, Time, sys_msec, uint64_t (*)());
	REGISTER_STATIC_METHOD(ID, Time, sys_usec, uint64_t (*)());
}

uint64_t Time::sys_msec()
{
	auto now = std::chrono::system_clock::now();

    // Convert the current time to time since epoch
    auto duration = now.time_since_epoch();

    // Convert duration to milliseconds
    auto milliseconds
        = std::chrono::duration_cast<std::chrono::milliseconds>(
              duration)
              .count();
	
	return milliseconds;
}

uint64_t Time::sys_usec() {
	auto now = std::chrono::system_clock::now();

    // Convert the current time to time since epoch
    auto duration = now.time_since_epoch();

    // Convert duration to milliseconds
    auto microseconds
        = std::chrono::duration_cast<std::chrono::microseconds>(
              duration)
              .count();
	
	return microseconds;
}

}