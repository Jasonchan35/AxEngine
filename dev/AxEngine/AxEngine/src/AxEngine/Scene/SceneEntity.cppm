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
	
	const String& name() const { return _name; }
	
	Int childCount() const { return _children.size(); }
	SceneEntity* childAt(Int index) { return _children[index].ptr(); }
	SceneEntity* operator[](Int index) { return _children[index].ptr(); }
	
	struct Editor {
		Editor() 
		: selected(false)
		, treeNodeIsOpen(true) 
		{}
		bool selected       : 1;
		bool treeNodeIsOpen : 1;
	} editor;
	
	SceneWorld* world() { return _world; }
	
protected:
	friend class SceneWorld;
	SceneWorld* _world = nullptr;
	
private:
	String _name;
	
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
	
	struct Selection {
		void select(SceneEntity* entity);
		void deselectAll();
		
		void getSelection(Array<SPtr<SceneEntity>> outList);
		
	private:
		Array<WPtr<SceneEntity>> _selectedEntities;
	} selection;

private:
	SPtr<SceneEntity> _root;
};

AX_CLASS()
class CTransform : public SceneComponent {
	AX_GENERATED_BODY()
	
	AX_PROP() Vec3f position;
	AX_PROP() Vec3f rotation;
	AX_PROP() Vec3f scale;
};

AX_CLASS()
class CMeshRenderer : public SceneComponent {
	AX_GENERATED_BODY()
public:	
	SPtr<MeshObject> mesh;
};

} // namespace