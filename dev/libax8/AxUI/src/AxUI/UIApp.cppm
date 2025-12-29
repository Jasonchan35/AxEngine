module;
export module AxUI:UIApp;
export import :Common;

export namespace AxUI {

struct UIApp_CreateDesc : public NativeUIApp_CreateDesc {
	RenderSystem::CreateDesc	renderSystemDesc;
};

class UIApp : public NativeUIApp {
	AX_RTTI_INFO(UIApp, NativeUIApp)
public:
	AX_DOWNCAST_GET_INSTANCE()

	using CreateDesc = UIApp_CreateDesc;
	UIApp(const CreateDesc& desc = CreateDesc()) : Base(desc) {
		_renderSystemDesc = desc.renderSystemDesc;
	}

	RenderSystem*	renderSystem() { return _renderSystem.ptr(); }

protected:
	virtual void onCreate() override;
	RenderSystem::CreateDesc	_renderSystemDesc;

private:
	UPtr<RenderSystem>			_renderSystem;
};

} // namespace