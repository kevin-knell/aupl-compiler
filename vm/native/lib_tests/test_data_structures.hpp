#pragma once

#include "vector.hpp"
#include "text_color.hpp"
#include "vec2.hpp"
#include "string.hpp"
#include "math.hpp"

using namespace auplib;

template<typename T>
void print_vector(Vector<T>& vector) {
	std::cout << "{ ";

	for (size_t i = 0; i < vector.size(); ++i) {
		if (i > 0) {
			std::cout << ", ";
		}
		std::cout << vector[i];
	}
	
	std::cout << " } " << vector.size() << " " << vector.capacity() << std::endl;
}

void test_vector() {
	std::cout << "testing vector" << std::endl;

	Vector<int> vector(5);

	vector.push(5);
	vector.push(6);
	vector.push(69);
	vector.push(42);

	print_vector(vector);

	vector.push(1337);
	vector.push(67);

	print_vector(vector);

	std::cout << vector[3, 4] << std::endl;
}

void test_vec2() {
	vec2 v(1.0, 3.0);
	vec2 w(2.0, 4.0);

	std::cout << v.linearly_independent(w) << std::endl;
}