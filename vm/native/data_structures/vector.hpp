#pragma once
#include <cstring>
#include <initializer_list>

namespace auplib {

template<typename T>
class Vector {
private:
	T* _data;
	size_t _size;
	size_t _capacity;

public:
	Vector() : _data(new T[16]), _size(0), _capacity(16) {}
	Vector(size_t cap) : _data(new T[cap]), _size(0), _capacity(cap) {}

	Vector(std::initializer_list<T> init_list) {
		reserve(init_list.size());
		std::memcpy(_data, init_list, sizeof(T) * init_list.size());
	}

	void reserve(size_t cap) {
		if (_capacity > cap) return;
		_capacity = cap;
		
		T* old_data = _data;
		T* new_data = new T[_capacity];

		std::memcpy(new_data, old_data, sizeof(T) * _size);
		delete[] old_data;

		_data = new_data;
	}

	void resize(size_t s) {
		if (_size > s) return;
		_size = s;
	}

	void push(T value) {
		if (_size >= _capacity) {
			reserve(_capacity * 2);
		}
		_data[_size++] = value;
	}

	T pop() {
		if (_size == 0) throw std::runtime_error("size of Vector cannot go below 0");
		return _data[--_size];
	}

	T* data() {
		return _data;
	}

	size_t size() {
		return _size;
	}

	size_t capacity() {
		return _capacity;
	}

	bool empty() {
		return _size == 0;
	}

	void clear() {
		_size = 0;
	}

	T& operator[](size_t idx) {
		return _data[idx];
	}

	T& operator[](size_t start_idx, size_t end_idx) {
		(void) end_idx;
		return _data[start_idx];
	}
};

}