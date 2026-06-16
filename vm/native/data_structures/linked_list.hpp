#pragma once

namespace auplib {

template<typename T>
class LinkedList {
	struct Slot {
		T element;
		T* next;
	};
};

} // namespace auplib
