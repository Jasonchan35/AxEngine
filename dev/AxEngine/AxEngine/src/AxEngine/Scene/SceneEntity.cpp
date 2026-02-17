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
		
		_world = _parent->getWorld();
	}

	_world->_onAddEntity(this);
	
	if (_parent) {
		_parent->_children.emplaceBack(this);
	}
}

template<class T> concept CON_HasMutRttiInit2 = requires(T::MutRttiInit2 obj)
{
	[](const T::MutRttiInit2&){}(obj); 
};

template<class SE>
void SceneEntity::_onJsonIO(SE& se) {
	AX_JSON_IO(se, _name);
	// AX_JSON_IO(se, metadata);
	AX_JSON_IO(se, _position);
	AX_JSON_IO(se, _rotation);
	AX_JSON_IO(se, _scale);
}

void SceneEntity::onJsonIO(JsonIO_Reader& se) { 
	markLocalMatrixDirty();
	_onJsonIO(se); 

	se.reader.readMemberArray("components", [&]() -> void {
		se.reader.beginObject();
		NameId compType;
		SceneComponent* comp = nullptr;
		while (!se.reader.endObject()) {
			if (se.reader.isMember("compType")) {
				se.io(compType);
				if (compType == MeshRendererComponent::s_rtti()->name) {
					comp = addComponent<MeshRendererComponent>(AX_NEW);
				}
				
				if (comp) {
					comp->onJsonIO(se);
				}
			}
		}
	});
	
	se.reader.readMemberArray("children", [&]() -> void {
		auto child = SceneEntity::s_new(AX_NEW, nullptr, this, "");
		se.io(*child);
	});
}

void SceneEntity::onJsonIO(JsonIO_Writer& se) { 
	_onJsonIO(se); 
	{
		auto compScope = se.writer.memberArrayScope("components");
		for (auto& comp : _components) {
			se.io(comp);
		}
	}
	{
		auto childScope = se.writer.memberArrayScope("children");
		for (auto& child : _children) {
			se.io(child);
		}
	}
}

template<class SE>
void SceneComponent::_onJsonIO(SE& se) {
	if constexpr (se.isReader()) {
	} else {
	}
}
void SceneComponent::onJsonIO(JsonIO_Reader& se) {
	_onJsonIO(se);
}; 
void SceneComponent::onJsonIO(JsonIO_Writer& se) {
	auto compType = rtti()->name;
	se.member_io("compType", compType);
	_onJsonIO(se);
}; 

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

void SceneWorld::readFromFile(StrView folder) {
	JsonIO::readFile(Fmt("{}/_root.axWorld", folder), *this);
}

void SceneWorld::writeToFile(StrView folder) {
	FileDir::create(folder);
	JsonIO::writeFile(Fmt("{}/_root.axWorld", folder), *this);
}

template<class SE> inline
void SceneWorld::_onJsonIO(SE& se) {

}

void SceneWorld::onJsonIO(JsonIO_Reader& se) {
	_onJsonIO(se);
	
	se.reader.readMemberArray("meshes", [&]() -> void {
		NameId meshName;
		se.io(meshName);
		auto newMesh = MeshObject::s_new(AX_NEW);
		newMesh->setName(meshName);
		_meshObjects.emplaceBack(newMesh);
		
		auto dir = Fmt("{}/meshes", FilePath::dirname(se.reader.filename()));
		newMesh->readMeshletFromFile(Fmt("{}/{}.axMeshlet", dir, meshName));
	});

	se.member_io("root", *_root);
}
void SceneWorld::onJsonIO(JsonIO_Writer& se) {
	_onJsonIO(se);

	se.writer.writeMemberArray("meshes", [&]() -> void {
		for (auto& mesh : _meshObjects) {
			if (!mesh) continue;
			auto dir = Fmt("{}/meshes", FilePath::dirname(se.writer.filename()));
			FileDir::create(dir);
			
			auto meshName = mesh->name();
			mesh->writeMeshletToFile(Fmt("{}/{}.axMeshlet", dir, meshName));
			se.io(meshName);
		}
	});
	
	se.member_io("root", *_root);
}

SceneWorld::SceneWorld() {
	_root.ref(SceneEntity::s_new(AX_NEW, this, nullptr, "root"));
	auto* mgr = RenderObjectManager::s_instance();
	_tileLightingBuffer.create(AX_NEW, "axGpuData_TileLighting", mgr->_structBufferPools.axGpuData_TileLighting);
}

void SceneWorld::_onAddEntity(SceneEntity* entity) {
	if (!entity->parent() && _root) {
		_root->_children.emplaceBack(entity);
	}
}

void MeshRendererComponent::onInit() {
	auto* world = getWorld();
	world->_meshRendererComponents.add(this);
}

MeshRendererComponent::~MeshRendererComponent() {
	auto* world = getWorld();
	world->_meshRendererComponents.remove(this);
}

template<class SE> inline
void MeshRendererComponent::_onJsonIO(SE& se) {
	Base::onJsonIO(se);
}

void MeshRendererComponent::onJsonIO(JsonIO_Reader& se) {
	_onJsonIO(se);
	NameId meshName;
	if (se.member_io("mesh", meshName)) {
		for (auto& p : getWorld()->_meshObjects) {
			if (p && p->name() == meshName) {
				this->mesh = p;
				this->material = RenderStockObjects::s_instance()->materials->meshlet; // TODO
				break;
			}
		}
	}
}
void MeshRendererComponent::onJsonIO(JsonIO_Writer& se) {
	_onJsonIO(se);
	if (mesh) {
		auto meshName = mesh->name();
		se.member_io("mesh", meshName);
	}
}

LightComponent::~LightComponent() {
	getWorld()->_lightComponents.remove(this);
}

void LightComponent::onInit() {
	Base::onInit();
	_lightObj = LightObject::s_new(AX_NEW);
	getWorld()->_lightComponents.add(this);
}

CameraComponent::~CameraComponent() {
	getWorld()->_cameraComponents.remove(this);
}

void CameraComponent::onInit() {
	Base::onInit();
	cameraObj = CameraObject::s_new(AX_NEW);
	getWorld()->_cameraComponents.add(this);
}

void SceneWorld::onRender(RenderRequest* req) {
	auto tileLightingSpan = _tileLightingBuffer.editData(0, 1);
	AxGpuData_TileLighting& tileLighting = tileLightingSpan[0];
	tileLighting = {};
	
	for (auto* comp : _lightComponents) {
		if (!comp) continue;

		auto* light = comp->_lightObj.ptr();
		if (!light) continue;

		light->setWorldPos(comp->entity()->worldPosition());

		if (tileLighting.lightCount >= AX_HLSL_MAX_ACTIVE_LIGHTS) {
			break; // TODO select most important light
		}
		
		tileLighting.lightIds[tileLighting.lightCount] = light->objectSlot.slotId();
		tileLighting.lightCount++;
	}
	_tileLightingBuffer.uploadAndGetOffset(req);

// ---- 	
	for (auto* comp : _meshRendererComponents) {
		if (!comp) continue;
		auto& worldMatrix = comp->entity()->worldMatrix();
		req->drawMesh(comp->mesh, comp->material, 0, worldMatrix);
	}
	
//	req->drawWorld(); // indirect draw
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

const Mat4f& SceneEntity::localMatrix() const {
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

const Mat4f& SceneEntity::worldMatrix() const {
	if (_worldMatrixDirty) {
		_worldMatrixDirty = false;
		_worldMatrix = _parent ? _parent->worldMatrix() * localMatrix() : localMatrix();
	}
	return _worldMatrix;
}

}