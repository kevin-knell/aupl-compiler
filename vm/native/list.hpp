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
	static void register_to_db(vm::ClassDB& db);

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

	T get(int64_t idx) {
		return data[idx];
	}

	void set(int64_t idx, T value) {
		data[idx] = value;
	}

	int64_t size() {
		return data.size();
	}

	bool empty() {
		return data.empty();
	}

	void clear() {
		data.clear();
	}
};

using List8 = List<int64_t>;

void register_list8_to_db(vm::ClassDB &db) {
	const int ID = REGISTER_CLASS(ID, List8);

	REGISTER_CONSTRUCTOR(ID, List8());
	
	REGISTER_METHOD(ID, List8, push, void (List8::*)(int64_t value));
	REGISTER_METHOD(ID, List8, pop, int64_t (List8::*)());
	REGISTER_METHOD(ID, List8, get, int64_t (List8::*)(int64_t idx));
	REGISTER_METHOD(ID, List8, set, void (List8::*)(int64_t idx, int64_t value));
	REGISTER_METHOD(ID, List8, size, int64_t (List8::*)());
	REGISTER_METHOD(ID, List8, empty, bool (List8::*)());
	REGISTER_METHOD(ID, List8, clear, void (List8::*)());
}