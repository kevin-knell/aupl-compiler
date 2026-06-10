#pragma once

#include "object.hpp"
#include <type_traits>
#include <cstdint>

namespace auplib {

template<class T>
class Shared;

template<class T>
class Weak;

class RefCounted : Object {
private:
protected:
	template<typename T>
	friend class Shared;
	
	struct RcKey {};

	explicit RefCounted(RcKey) {}
    virtual ~RefCounted() = default;

private:
    mutable uint32_t ref_count{0};
};

} // namespace auplib
