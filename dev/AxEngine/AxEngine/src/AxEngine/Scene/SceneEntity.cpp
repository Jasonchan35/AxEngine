module AxEngine;
import :SceneEntity;

namespace AxEngine {

SPtr<SceneWorld> SceneWorld::s_new(const MemAllocRequest& allocReq) {
	return new(allocReq) SceneWorld();
} 

SceneEntity::SceneEntity(const CreateDesc& desc) {
	setName(desc.name);
	_world  = desc.world;
	_parent = desc.parent;
	
	if (!_world) {
		if (!_parent) {
			throw Error_Undefined("missing world and parant in SceneEntity_CreateDesc");
		}
		
		_world = _parent->world();
	}

	_world->_onAddEntity(this);
	
	if (_parent) {
		_parent->_children.emplaceBack(this);
	}
}

SPtr<SceneEntity> SceneEntity::s_new(const MemAllocRequest& allocReq,
                                     SceneWorld*            world,
                                     SceneEntity*           parent,
                                     StrView                name) {
	CreateDesc desc;
	desc.name   = name;
	desc.parent = parent;
	desc.world  = world;
	return SPtr_new<This>(allocReq, desc);
}

SceneWorld::SceneWorld() {
	_meshRendererSystem = SPtr_new<MeshRendererSystem>(AX_NEW);
	_root.ref(SceneEntity::s_new(AX_NEW, this, nullptr, "root"));
}

void SceneWorld::_onAddEntity(SceneEntity* entity) {
	if (!entity->parent() && _root) {
		_root->_children.emplaceBack(entity);
	}
}

void MeshRendererComponent::onInit() {
	auto* sys = world()->meshRendererSystem();
	_systemSlotId = sys->_componentList.size();
	sys->_componentList.emplaceBack(this);
}

MeshRendererComponent::~MeshRendererComponent() {
	if (auto* sys = world()->meshRendererSystem()) {
		auto& slot = sys->_componentList[_systemSlotId];
		AX_ASSERT(slot == this);
		sys->_componentList.eraseAt_Unordered(_systemSlotId);
		slot->_systemSlotId = _systemSlotId;
	}
}

void MeshRendererSystem::onRender(RenderRequest* req) {
	req->drawWorld(); // indirect draw
	
	for (auto* comp : _componentList) {
		if (!comp) continue;
		auto& objectToWorld = comp->entity()->worldMatrix();
		req->drawMesh(comp->mesh, comp->material, 0, objectToWorld);
	}
}

void SceneEntity::setWorldMatrix(const Mat4f& mat) {
	if (_parent) {
		setLocalMatrix(_parent->worldMatrix().inverse() * mat);
	} else {
		setLocalMatrix(mat);
	}
}

const Mat4f& SceneEntity::worldMatrix() {
	// TODO dirty _worldMatrix
	_worldMatrix = _parent ? _parent->worldMatrix() * localMatrix() : localMatrix();
	return _worldMatrix;
}

}