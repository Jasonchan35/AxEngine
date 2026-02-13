module;

export module AxEngine:Engine;
export import :Common;
export import :SceneEntity;

export namespace AxEngine {

class SceneWorld;
class Engine : public RttiObject {
	AX_RTTI_INFO(Engine, RttiObject)
public:
	Engine();
	virtual ~Engine() override;

	static Engine* s_instance();

	struct CreateDesc {
		bool inEditor = false;
	};

	void create(const CreateDesc& desc);

	bool inEditor() const { return _inEditor; }

	void createWorld();
	
	void setWorld(SceneWorld* world) { _world = world; }
	SceneWorld* world() const { return _world; }
private:
	bool _inEditor = false;
	SPtr<SceneWorld>	_world;
};

} //namespace
