#pragma once

#include "object.hpp"
#include "native.hpp"
#include "value.hpp"
#include <vector>
#include "class_db.hpp"

namespace vm {
    class ClassDB;
} // namespace vm

namespace auplib {

template<typename T>
class List : public Object {
public:
	static void register_to_db(vm::ClassDB& db);

private:
	std::vector<T> data;

public:
	List() {}

	List(std::initializer_list<T> init_list) {
		data = init_list;
	}

	template<typename U>
	List(std::initializer_list<U> init_list)
		requires std::is_convertible_v<U, T>
	{
		for (const auto& x : init_list)
			push(x);
	}

	template<typename U>
    List(const List<U>& other)
        	requires std::is_convertible_v<U, T>
	{
		for (size_t i = 0; i < other.size(); ++i) {
			push(other[i]);
		}
    }

	void push(T value) {
		data.push_back(value);
	}

	T pop() {
		T back = data.back();
		data.pop_back();
		return back;
	}

	T get(size_t idx) {
		return data[idx];
	}

	void set(size_t idx, T value) {
		data[idx] = value;
	}

	size_t size() {
		return data.size();
	}

	bool empty() {
		return data.empty();
	}

	void clear() {
		data.clear();
	}

	T& operator[](size_t idx) {
		return data[idx];
	}
};

}