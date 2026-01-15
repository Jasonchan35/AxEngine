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

SPtr<SceneEntity> SceneEntity::s_new(const MemAllocRequest& allocReq, SceneEntity* parent, StrView name) {
	return SPtr_new<This>(allocReq, CreateDesc(parent, name));
}

SceneWorld* SceneWorld::s_instance() {
	return SceneWorld_instance;
}

SceneWorld::SceneWorld() {
	SceneWorld_instance = this;
	_root.ref(SceneEntity::s_new(AX_NEW, nullptr, "root"));
}

void SceneWorld::Selection::select(SceneEntity* entity) {
	deselectAll();
	entity->editor.selected = true;
	_selectedEntities.emplaceBack(entity);
}

void SceneWorld::Selection::deselectAll() {
	for (auto& p : _selectedEntities) {
		if (auto sp = p.getSPtr()) {
			sp->editor.selected = false;
		}
	}
	_selectedEntities.clear();
}

void SceneWorld::Selection::getSelection(Array<SPtr<SceneEntity>> outList) {
	outList.clear();
	outList.ensureCapacity(_selectedEntities.size());
	for (auto& p : _selectedEntities) {
		if (auto sp = p.getSPtr()) {
			outList.emplaceBack(std::move(sp));
		}
	}
}

}