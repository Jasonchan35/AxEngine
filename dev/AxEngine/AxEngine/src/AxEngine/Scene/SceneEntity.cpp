module AxEngine;
import :SceneEntity;

namespace AxEngine {

SceneWorld* SceneWorld_instance = nullptr;

SceneEntity::SceneEntity(const CreateDesc& desc) {
	setName(desc.name);
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

MeshRendererComponent::MeshRendererComponent() {
	auto* sys = MeshRendererSystem::s_instance(true);
	_systemSlotId = sys->_componentList.size();
	sys->_componentList.emplaceBack(this);
}

MeshRendererComponent::~MeshRendererComponent() {
	auto* sys = MeshRendererSystem::s_instance(true);
	auto& slot = sys->_componentList[_systemSlotId];
	AX_ASSERT(slot == this);
	sys->_componentList.eraseAt_Unordered(_systemSlotId);
	slot->_systemSlotId = _systemSlotId;
}

MeshRendererSystem* MeshRendererSystem::s_instance(bool createIfNone) {
	SceneWorld* world = SceneWorld_instance;
	if (!world) return nullptr;

	if (!world->_meshRendererSystem && createIfNone) {
		world->_meshRendererSystem = UPtr_new<MeshRendererSystem>(AX_NEW);
	}

	return world->_meshRendererSystem.ptr();
}

void MeshRendererSystem::onRender(RenderRequest* req) {
	for (auto* comp : _componentList) {
		if (!comp) continue;
		auto& objectToWorld = comp->entity()->worldMatrix();
		
		auto& mr = comp->renderer;
		
		mr.objectSlot.markDirty();
		mr._gpuData.worldMatrix = objectToWorld;
		
		if (!mr.mesh || !mr.material) return;
		req->drawMesh(mr.mesh, mr.material, 0, objectToWorld);
	}
	
}; 

}