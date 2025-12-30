module;
export module AxImUI:ImUIApp;
export import :Common;

export namespace AxUI {

struct ImuiApp_CreateDesc : public NativeUIApp_CreateDesc {
	RenderSystem::CreateDesc	renderSystemDesc;
};

class ImUIApp : public NativeUIApp {
	AX_RTTI_INFO(ImUIApp, NativeUIApp)
public:
	AX_DOWNCAST_GET_INSTANCE()

	using CreateDesc = ImuiApp_CreateDesc;
	ImUIApp(const CreateDesc& desc = CreateDesc()) : Base(desc) {
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