module;

#include "SceneEntity.gen.h"

export module AxEngine:SceneEntity;
export import :Object;

export namespace AxEngine {

class SceneWorld;

AX_CLASS()
class SceneComponent : public Object {
	AX_GENERATED_BODY()
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

AX_CLASS()
class SceneWorld : public Object {
	AX_GENERATED_BODY()
public:
	static SceneWorld* s_instance();
	
	SceneWorld();
	SceneEntity* root() { return _root.ptr(); }
	
private:
	SPtr<SceneEntity> _root;
};

AX_CLASS()
class CMeshRenderer : public SceneComponent {
	AX_GENERATED_BODY()
public:	
	SPtr<MeshObject> mesh;
};

} // namespace