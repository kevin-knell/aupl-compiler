#include "scene.hpp"

namespace auplib
{

void Scene::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_CLASS(Scene);
	REGISTER_CONSTRUCTOR(ID, Scene());
	REGISTER_VARIABLE(ID, Shared<Node>, root);
}

}