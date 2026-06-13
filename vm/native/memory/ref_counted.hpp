#pragma once

#include "object.hpp"
#include <type_traits>
#include <cstdint>

namespace auplib {

template<class T>
class Shared;

template<class T>
class Weak;

class RefCounted : public Object {
private:
protected:
	template<typename T>
	friend class Shared;
	
	struct RcKey {};

	explicit RefCounted(RcKey) {}

private:
    mutable uint32_t ref_count{0};

public:
	static void register_to_db(vm::ClassDB &db);
};

} // namespace auplib
