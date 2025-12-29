module;

export module AxEngine:Engine;
export import :Common;

export namespace ax::AxEngine {

AX_CLASS()
class Engine : public RttiObject {
	AX_RTTI_INFO(Engine, RttiObject)
public:
	Engine();
	virtual ~Engine() override;

	static Engine* s_instance();

	struct CreateDesc {
		bool inEditor = false;
	};

	void create(CreateDesc& desc);

	bool inEditor() const { return _inEditor; }

private:
	bool _inEditor = false;
};

} //namespace
