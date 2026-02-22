module;

#include "SceneEntity-gen.h"

export module AxEngine:SceneEntity;
export import :Object;

export namespace AxEngine {

template<class T>
class ObjectTableSlot : public NonCopyable {
public:
	~ObjectTableSlot() { AX_ASSERT(_index == -1); }
	
	// protected:
	Int _index = -1;
};

template<class T, ObjectTableSlot<T> T::*pSlot>
class ObjectTable {
public:
	void add(T* obj) {
		auto& index = (obj->*pSlot)._index;
		AX_ASSERT(index < 0);
		
		if (_freeSlots.size() > 0) {
			index = _freeSlots.popBack();
			_list[index] = obj;
		} else {
			index = _list.size();
			_list.emplaceBack(obj);
		}
	}
	
	void remove(T* obj) {
		auto& index = (obj->*pSlot)._index;
		AX_ASSERT(index >= 0);
		AX_ASSERT(_list[index] == obj);
		_list[index] = nullptr;
		_freeSlots.emplaceBack(index);
		index = -1;
		
	}
	
	Span<T*>	list() { return _list; }
	
	auto begin(){ return _list.begin();  }
	auto end()	 { return _list.end();  }
	
private:
	Array<T*>	_list;
	Array<Int>	_freeSlots;
};

class SceneEntity;
class SceneWorld;

AX_CLASS()
class SceneComponent : public Object {
	AX_GENERATED_BODY()
public:
	      SceneEntity* entity()       { return _entity; }
	const SceneEntity* entity() const { return _entity; }

	SceneWorld* getWorld() const;
	
	virtual void onJsonIO(JsonIO_Reader& se);
	virtual void onJsonIO(JsonIO_Writer& se);

	virtual void onDrawGizmo(ImUIDrawGizmoRequest* req) {}
	
protected:
	template<class SE> void _onJsonIO(SE& se);
	
	friend class SceneEntity;
	
	void _init(SceneEntity* entity) { _entity = entity; onInit(); }
	virtual void onInit() {}
	
	SceneEntity* _entity = nullptr;
};

class SceneEntity_CreateDesc {
public:
	SceneWorld*  world  = nullptr;
	SceneEntity* parent = nullptr;
	NameId name;
};

AX_CLASS()
class SceneEntity : public Object {
	AX_GENERATED_BODY()
public:
	using CreateDesc = SceneEntity_CreateDesc;
	SceneEntity(const CreateDesc& desc);

	static SPtr<SceneEntity> s_new(const MemAllocRequest& allocReq,
	                               SceneWorld*            world,
	                               SceneEntity*           parent,
	                               InNameId               name);

	template<class COMP>
	COMP* addComponent(const MemAllocRequest& allocReq) {
		auto comp = SPtr_new<COMP>(allocReq);
		_components.emplaceBack(comp);
		comp->_init(this);
		return comp;
	}
	
	template<class COMP>
	COMP* getComponent() {
		for (auto& comp : _components) {
			if (auto* c = rttiCast<COMP>(comp.ptr()))
				return c;
		}
		return nullptr;
	}
	
	SceneWorld* getWorld() const { return _world; }
	
	Int childCount() const { return _children.size(); }
	SceneEntity* childAt(Int index) { return _children[index].ptr(); }
	void ensureChildrenCapacity(Int n) { _children.ensureCapacity(n); }

	Int componentCount() const { return _components.size(); }
	SceneComponent* componentAt(Int index) { return _components[index].ptr(); }
	
	SceneEntity* findChild(NameId name, bool recursive);

	const Vec3f&  position() const { return _position; }
	const Quat4f& rotation() const { return _rotation; }
	const Vec3f&  scale() const { return _scale; }
	
	void setPosition(const Vec3f&  pos  ) { _position = pos;   markLocalMatrixDirty(); }
	void setPosition(float x, float y, float z) { setPosition(Vec3f(x,y,z)); }
	
	void setRotation(const Quat4f& rot  ) { _rotation = rot;   markLocalMatrixDirty(); }
	void setRotation(float x, float y, float z) { setRotation(Quat4f::s_euler(x,y,z)); }
	
	void setScale   (const Vec3f&  scale) { _scale    = scale; markLocalMatrixDirty(); }
	void setScale   (float x, float y, float z) { setScale(Vec3f(x,y,z)); }

	void setTRS(const Vec3f& position_, const Quat4f& rotation_, const Vec3f& scale_) {
		_position = position_;
		_rotation = rotation_;
		_scale    = scale_;
		markLocalMatrixDirty();
	};
	
	Vec3f worldPosition() const { return worldMatrix().position(); }
	
	void markWorldMatrixDirty();
	void markLocalMatrixDirty();

	void setLocalMatrix(const Mat4f& mat);

	const Mat4f& localMatrix() const;

	void setWorldMatrix(const Mat4f& mat);
	const Mat4f& worldMatrix() const;

	SceneEntity* parent()       { return _parent; }
	const SceneEntity* parent() const { return _parent; }
	
	JsonValue metadata = JsonValue::s_null();
	
	void onJsonIO(JsonIO_Reader& se);
	void onJsonIO(JsonIO_Writer& se);

protected:
	template<class SE> void _onJsonIO(SE& se);

	friend class SceneWorld;

	Array<SPtr<SceneComponent>> _components;
	Array<SPtr<SceneEntity>>    _children;

	SceneWorld*  _world    = nullptr;
	SceneEntity* _parent   = nullptr;
	Vec3f        _position = Vec3f::s_zero();
	Quat4f       _rotation = Quat4f::s_identity();
	Vec3f        _scale    = Vec3f::s_one();
	
	mutable Mat4f _localMatrix;
	mutable Mat4f _worldMatrix;
	mutable bool  _localMatrixDirty : 1;
	mutable bool  _worldMatrixDirty : 1;
};

class MeshRendererSystem;

AX_CLASS()
class MeshRendererComponent : public SceneComponent {
	AX_GENERATED_BODY()
public:	
	virtual void onInit() override;
	virtual ~MeshRendererComponent() override;

	SPtr<MeshObject> mesh;
	SPtr<Material>   material;
	
	virtual void onJsonIO(JsonIO_Reader& se) override;
	virtual void onJsonIO(JsonIO_Writer& se) override;
	
	ObjectTableSlot<This> _slot;
	
private:
	template<class SE> void _onJsonIO(SE& se);
	
	Int _systemSlotId = 0;
};

inline SceneWorld* SceneComponent::getWorld() const { return _entity->getWorld(); }; 

AX_CLASS()
class LightComponent : public SceneComponent {
	AX_GENERATED_BODY()
public:
	~LightComponent();

	ObjectTableSlot<This> _slot;

	SPtr<LightObject> lightObj;
	
protected:
	virtual void onInit() override;
	virtual void onDrawGizmo(ImUIDrawGizmoRequest* req) override;
};

AX_CLASS()
class CameraComponent : public SceneComponent {
	AX_GENERATED_BODY()
public:
	~CameraComponent();

	ObjectTableSlot<This> _slot;
	
	SPtr<CameraObject> cameraObj;

protected:
	virtual void onInit() override;
	
};


AX_CLASS()
class SceneWorld : public Object {
	AX_GENERATED_BODY()
public:
	static SPtr<SceneWorld> s_new(const MemAllocRequest& allocReq);

	SceneEntity* root() { return _root.ptr(); }
	
	bool readFromFile(StrView folder);
	void writeToFile(StrView folder);
	
	void onJsonIO(JsonIO_Reader& se);
	void onJsonIO(JsonIO_Writer& se);
	
	Array<SPtr<MeshObject>> _meshObjects;
	
	void onRender(RenderRequest* req);
	
protected:
	friend class SceneEntity;
	friend class MeshRendererComponent;
	friend class LightComponent;
	friend class CameraComponent;
	
	template<class SE> void _onJsonIO(SE& se);
	
	SceneWorld();
	void _onAddEntity(SceneEntity* entity);
	
	ObjectTable<CameraComponent,       &CameraComponent::_slot      > _cameraComponents;
	ObjectTable<LightComponent,        &LightComponent::_slot       > _lightComponents;
	ObjectTable<MeshRendererComponent, &MeshRendererComponent::_slot> _meshRendererComponents;
	
	StructuredGpuBuffer_<AxGpuData_TileLighting>		_tileLightingBuffer;

	static u32 s_fileVersion();
	u32 _fileVersion = 0;
	
	SPtr<SceneEntity> _root;
};

} // namespace