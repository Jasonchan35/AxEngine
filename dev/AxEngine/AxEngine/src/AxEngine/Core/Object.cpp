module;

module AxEngine;
import :Object;

namespace AxEngine {

ObjectManager* ObjectManager::s_instance() {
	static GlobalSingleton<ObjectManager> s;
	return s.ptr();
}

void ObjectManager::_addType(const Rtti* rtti) {
	_types.append(rtti);
}

} // namespace