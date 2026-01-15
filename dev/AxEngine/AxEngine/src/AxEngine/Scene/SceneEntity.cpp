module AxEngine;
import :SceneEntity;

namespace AxEngine {

SceneWorld* SceneWorld_instance = nullptr;

SceneEntity::SceneEntity(const CreateDesc& desc) {
	_name = desc.name;
	_parent = desc.parent;
	if (!_parent) {
		_parent = SceneWorld_instance->root();
	}
	
	if (_parent) {
		_parent->_children.emplaceBack().ref(this);
	}
}

SceneEntity* SceneEntity::s_new(const MemAllocRequest& allocReq, SceneEntity* parent, StrView name) {
	return UPtr_new<This>(allocReq, CreateDesc(parent, name)).detach();
}

SceneWorld* SceneWorld::s_instance() {
	return SceneWorld_instance;
}

SceneWorld::SceneWorld() {
	SceneWorld_instance = this;
	_root.ref(SceneEntity::s_new(AX_NEW, nullptr, "root"));
}

}