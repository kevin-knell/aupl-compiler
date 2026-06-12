#include "scene.hpp"

namespace auplib
{

void Scene::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_OBJECT_CLASS(Scene, Object);
	REGISTER_CONSTRUCTOR(ID, Scene());
	REGISTER_SETGET(ID, Scene, Shared<Node>, root);
}

}