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
	void ensureChildrenCapacity(Int n) { _children.ensureCapacity(n); }

	Int componentCount() const { return _components.size(); }
	SceneComponent* componentAt(Int index) { return _components[index].ptr(); }

	struct Transform {
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
	private:
		Mat4f _localMatrix;
	};
	
	Transform transform;
	
	const Mat4f& worldMatrix() {
		// TODO dirty _worldMatrix
		_worldMatrix = transform.localMatrix();
		if (_parent) _worldMatrix *= _parent->worldMatrix();
		return _worldMatrix;
	}
	
		  SceneEntity* parent()       { return _parent; }
	const SceneEntity* parent() const { return _parent; }
	
protected:
	friend class SceneWorld;
	SceneWorld* _world = nullptr;
	
private:
	Array<SPtr<SceneComponent>> _components;
	Array<SPtr<SceneEntity>> _children;
	SceneEntity* _parent = nullptr;
	Mat4f _worldMatrix;
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