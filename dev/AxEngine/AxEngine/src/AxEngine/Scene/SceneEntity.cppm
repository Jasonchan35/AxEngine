module;

#include "SceneEntity.gen.h"

export module AxEngine:SceneEntity;
export import :Object;

export namespace AxEngine {

AX_CLASS()
class SceneComponent : public Object {
	AX_GENERATED_BODY()
};

AX_CLASS()
class SceneEntity : public Object {
	AX_GENERATED_BODY()
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
class TransformComponent : public SceneComponent {
	AX_GENERATED_BODY()
	
	AX_PROP() Vec3f position;
	AX_PROP() Vec3f rotation;
	AX_PROP() Vec3f scale;
};

AX_CLASS()
class RenderMeshComponent : public SceneComponent {
	AX_GENERATED_BODY()
	
};

} // namespace