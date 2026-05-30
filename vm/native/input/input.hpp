#pragma once

#include "native.hpp"
#include <GLFW/glfw3.h>

namespace auplib {

enum class Key {
	W = GLFW_KEY_W,
	S = GLFW_KEY_S,
	UP = GLFW_KEY_UP,
	DOWN = GLFW_KEY_DOWN
};

class Input {
public:
	static GLFWwindow* window;
	static void register_to_db(vm::ClassDB& db);
	static bool is_key_down(int key);
};

} // namespace auplib
