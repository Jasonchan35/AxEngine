module;

export module AxEngine:SceneEntity;
export import :Object;

export namespace AxEngine {

AX_CLASS()
class SceneComponent : public Object {
	
};

AX_CLASS()
class SceneEntity : public Object {
	AX_RTTI_INFO(SceneEntity, Object)
public:
	struct CreateDesc {
		CreateDesc(StrView name_) : name(name_) {};
		StrView name;
	};
	SceneEntity(const CreateDesc& desc) {}

	static SceneEntity* s_new(const MemAllocRequest& allocReq, StrView name) {
		return UPtr_new<This>(allocReq, CreateDesc(name)).detach();
	}
	
	template<class COMP>
	COMP* addComponent(const MemAllocRequest& allocReq) {
		auto comp = UPtr_new<COMP>(allocReq);
		return static_cast<COMP*>(_components.emplaceBack(std::move(comp)).ptr());
	}
	
private:
	Array<UPtr<SceneComponent>> _components;
};

AX_CLASS()
class RenderMeshComponent : public SceneComponent {
public:
	
};

} // namespace