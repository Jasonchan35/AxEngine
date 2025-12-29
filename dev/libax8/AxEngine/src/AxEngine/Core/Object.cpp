module;

module AxEngine;
import :Object;

namespace AxEngine {

ObjectManager* ObjectManager::s_instance() {
	static GlobalSingleton<ObjectManager> s;
	return s.ptr();
}

} // namespace