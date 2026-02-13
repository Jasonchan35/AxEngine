module AxEngine;
import :SceneEntity;

namespace AxEngine {

SPtr<SceneWorld> SceneWorld::s_new(const MemAllocRequest& allocReq) {
	return new(allocReq) SceneWorld();
} 

SceneEntity::SceneEntity(const CreateDesc& desc)
: _localMatrixDirty(false)
, _worldMatrixDirty(false)
{
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

void SceneEntity::markWorldMatrixDirty() {
	if (_worldMatrixDirty) return;
	_worldMatrixDirty = true;
	for (auto& child : _children) {
		child->markWorldMatrixDirty();
	}
}

void SceneEntity::markLocalMatrixDirty() {
	if (_localMatrixDirty) return;
	_localMatrixDirty = true;
	markWorldMatrixDirty();
}

void SceneEntity::setLocalMatrix(const Mat4f& mat) {
	auto trs = mat.getTRS();
	_position = trs.position;
	_rotation = trs.rotation;
	_scale    = trs.scale;
	markLocalMatrixDirty();
}

const Mat4f& SceneEntity::localMatrix() {
	if (_localMatrixDirty) {
		_localMatrixDirty = false;
		_localMatrix.setTRS(_position, _rotation, _scale);
	}
	return _localMatrix;
}

void SceneEntity::setWorldMatrix(const Mat4f& mat) {
	if (_parent) {
		setLocalMatrix(_parent->worldMatrix().inverse() * mat);
	} else {
		setLocalMatrix(mat);
	}
}

const Mat4f& SceneEntity::worldMatrix() {
	if (_worldMatrixDirty) {
		_worldMatrixDirty = false;
		_worldMatrix = _parent ? _parent->worldMatrix() * localMatrix() : localMatrix();
	}
	return _worldMatrix;
}

}