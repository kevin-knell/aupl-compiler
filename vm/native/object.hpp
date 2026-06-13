#pragma once

#include "native.hpp"

#define OBJECT_CLASS(m_class, m_parent)														\
public:																						\
	static void* get_class_ptr_static() {													\
		static int ptr;																		\
		return& ptr;																		\
	}																						\
	virtual bool is_class_ptr(void* ptr) const override {									\
		return (ptr == get_class_ptr_static()) || m_parent::is_class_ptr(ptr);				\
	}

namespace auplib {

template<typename T>
class Shared;

class Object {
public:
	static void register_to_db(vm::ClassDB& db);
    virtual ~Object() = default;
	//virtual String to_string() { return this; };

	static void* get_class_ptr_static() {
		static int ptr;
		return& ptr;
	}

	virtual bool is_class_ptr(void* ptr) const {
		return ptr == get_class_ptr_static();
	}

	template <typename T>
	static T* cast_to(Object* object) {
		static_assert(std::is_base_of_v<Object, T>, "T must be derived from Object");
		return object && object->is_class_ptr(T::get_class_ptr_static()) ? static_cast<T*>(object) : nullptr;
	}
};

}