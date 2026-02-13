module;

#include "SceneEntity-gen.h"

export module AxEngine:SceneEntity;
export import :Object;

export namespace AxEngine {

class SceneEntity;
class SceneWorld;

AX_CLASS()
class SceneComponent : public Object {
	AX_GENERATED_BODY()
public:
	      SceneEntity* entity()       { return _entity; }
	const SceneEntity* entity() const { return _entity; }

	SceneWorld* world() const;

protected:
	friend class SceneEntity;
	
	void _init(SceneEntity* entity) { _entity = entity; onInit(); }
	virtual void onInit() {}
	
	SceneEntity* _entity = nullptr;
};

class SceneEntity_CreateDesc {
public:
	SceneWorld*  world  = nullptr;
	SceneEntity* parent = nullptr;
	StrView name;
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
	                               StrView                name);

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
			if (auto* c = rttiCastCheck<COMP>(comp.ptr()))
				return c;
		}
		return nullptr;
	}
	
	SceneWorld* world() const { return _world; }
	
	Int childCount() const { return _children.size(); }
	SceneEntity* childAt(Int index) { return _children[index].ptr(); }
	void ensureChildrenCapacity(Int n) { _children.ensureCapacity(n); }

	Int componentCount() const { return _components.size(); }
	SceneComponent* componentAt(Int index) { return _components[index].ptr(); }

	Vec3f  position = Vec3f::s_zero();
	Quat4f rotation = Quat4f::s_identity();
	Vec3f  scale    = Vec3f::s_one();

	void setTRS(const Vec3f& position_, const Quat4f& rotation_, const Vec3f& scale_) {
		position = position_;
		rotation = rotation_;
		scale    = scale_;
	};
		
	void setLocalMatrix(const Mat4f& mat) { mat.getTRS(position, rotation, scale); }
	
	const Mat4f& localMatrix() {
		_localMatrix.setTRS(position, rotation, scale);
		return _localMatrix;
	}

	void setWorldMatrix(const Mat4f& mat);
	const Mat4f& worldMatrix();

	SceneEntity* parent()       { return _parent; }
	const SceneEntity* parent() const { return _parent; }
	
	JsonValue metadata;
	
protected:
	friend class SceneWorld;
	
	Array<SPtr<SceneComponent>> _components;
	Array<SPtr<SceneEntity>> _children;
	SceneWorld*  _world = nullptr;
	SceneEntity* _parent = nullptr;
	Mat4f _worldMatrix;
	Mat4f _localMatrix;
};

class MeshRendererSystem;

AX_CLASS()
class SceneWorld : public Object {
	AX_GENERATED_BODY()
public:
	static SPtr<SceneWorld> s_new(const MemAllocRequest& allocReq);

	SceneEntity* root() { return _root.ptr(); }
	
	MeshRendererSystem* meshRendererSystem() { return _meshRendererSystem.ptr(); }
protected:
	friend class SceneEntity;
	
	SceneWorld();
	SPtr<MeshRendererSystem> _meshRendererSystem;
	
	void _onAddEntity(SceneEntity* entity);
	
private:
	SPtr<SceneEntity> _root;
};

AX_CLASS()
class MeshRendererComponent : public SceneComponent {
	AX_GENERATED_BODY()
public:	
	virtual void onInit() override;
	virtual ~MeshRendererComponent() override;

	SPtr<MeshObject> mesh;
	SPtr<Material>   material;
	
private:
	Int _systemSlotId = 0;
};

AX_CLASS()
class MeshRendererSystem : public Object {
	AX_GENERATED_BODY()
public:
	void onRender(RenderRequest* req);
	
protected:
	friend class MeshRendererComponent;
	Array<MeshRendererComponent*> _componentList;
};

inline SceneWorld* SceneComponent::world() const { return _entity->world(); }; 

} // namespace