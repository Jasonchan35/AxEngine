module;

#include "SceneEntity.gen.h"

export module AxEngine:SceneEntity;
export import :Object;

export namespace AxEngine {

class SceneWorld;
class SceneEntity;

AX_CLASS()
class SceneComponent : public Object {
	AX_GENERATED_BODY()
public:
	      SceneEntity* entity()       { return _entity; }
	const SceneEntity* entity() const { return _entity; }

protected:
	friend class SceneEntity;
	SceneEntity* _entity = nullptr;
};

AX_CLASS()
class SceneEntity : public Object {
	AX_GENERATED_BODY()
public:
	struct CreateDesc {
		CreateDesc(SceneEntity* parent_, StrView name_) 
		: parent(parent_)
		, name(name_) {}
		
		SceneEntity* parent = nullptr;
		StrView name;
	};
	
	SceneEntity(const CreateDesc& desc);

	static SPtr<SceneEntity> s_new(const MemAllocRequest& allocReq, SceneEntity* parent, StrView name);

	template<class COMP>
	COMP* addComponent(const MemAllocRequest& allocReq) {
		auto comp = SPtr_new<COMP>(allocReq);
		_components.emplaceBack(comp);
		comp->_entity = this;
		return comp;
	}
	
	SceneWorld* world() { return _world; }
	
	Int childCount() const { return _children.size(); }
	SceneEntity* childAt(Int index) { return _children[index].ptr(); }

	Int componentCount() const { return _components.size(); }
	SceneComponent* componentAt(Int index) { return _components[index].ptr(); }

	struct Transform {
		Vec3f position{0,0,0};
		Vec3f rotation{0,0,0};
		Vec3f scale{1,1,1};
		
		const Mat4f& getObjectToWorld() {
			_objectToWorld = Mat4f::s_TRS_deg(position, rotation, scale);
			return _objectToWorld;
		}
	private:
		Mat4f _objectToWorld;
	};
	
	Transform transform;
	
protected:
	friend class SceneWorld;
	SceneWorld* _world = nullptr;
	
private:
	Array<SPtr<SceneComponent>> _components;
	Array<SPtr<SceneEntity>> _children;
	SceneEntity* _parent = nullptr;
};

class CMeshRendererSystem;

AX_CLASS()
class SceneWorld : public Object {
	AX_GENERATED_BODY()
public:
	static SceneWorld* s_instance();
	
	SceneWorld();
	SceneEntity* root() { return _root.ptr(); }

	UPtr<CMeshRendererSystem> _meshRendererSystem;
	
private:
	SPtr<SceneEntity> _root;
};

AX_CLASS()
class CMeshRenderer : public SceneComponent {
	AX_GENERATED_BODY()
public:	
	CMeshRenderer();
	virtual ~CMeshRenderer() override;
	
	SPtr<MeshObject> mesh;
	SPtr<Material> material;
	
	Int _systemSlotId;
};

AX_CLASS()
class CMeshRendererSystem : public Object {
	AX_GENERATED_BODY()
public:
	static CMeshRendererSystem* s_instance(bool createIfNone);
	
	void onRender(RenderRequest* req);
	
protected:
	friend class CMeshRenderer;
	Array<CMeshRenderer*> _componentList;
};

} // namespace