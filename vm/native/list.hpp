#pragma once
#include "object.hpp"
#include "native.hpp"
#include "value.hpp"
#include <vector>

namespace vm {
    class ClassDB;
} // namespace vm

template<typename T>
class List : public Object {
public:
	static void register_to_db(vm::ClassDB& db) {
		REGISTER_CLASS(4, List<vm::Value>);
		REGISTER_CLASS(5, List<vm::Value2>);
		REGISTER_CLASS(6, List<vm::Value4>);
		REGISTER_CLASS(7, List<vm::Value8>);
		REGISTER_CLASS(8, List<vm::Value16>);
	}

private:
	std::vector<T> data;

public:
	void push(T value) {
		data.push_back(value);
	}

	T pop() {
		T back = data.back();
		data.pop_back();
		return back;
	}
};