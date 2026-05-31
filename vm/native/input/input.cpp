#include "input.hpp"

#include "class_db.hpp"

namespace auplib {

/*
namespace {
	int translate(Key key) {
		return static_cast<int>(key);
	}
} // namespace
*/

GLFWwindow* Input::window;

void Input::register_to_db(vm::ClassDB& db) {
	const uint16_t ID = REGISTER_CLASS(Input);

	REGISTER_STATIC_METHOD(ID, Input, is_key_down, bool (*)(int key));
}

bool Input::is_key_down(int key) {
	return glfwGetKey(window, (key)) == GLFW_PRESS;
}
}