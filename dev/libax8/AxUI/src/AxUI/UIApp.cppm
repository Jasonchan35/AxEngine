module;
export module AxUI:UIApp;
export import :PCH;

export namespace ax::AxUI {

struct UIApp_CreateDesc : public NativeUIApp_CreateDesc {
	Renderer::CreateDesc	rendererDesc;
};

class UIApp : public NativeUIApp {
	AX_RTTI_INFO(UIApp, NativeUIApp)
public:
	AX_DOWNCAST_GET_INSTANCE()

	using CreateDesc = UIApp_CreateDesc;
	UIApp(const CreateDesc& desc = CreateDesc()) : Base(desc) {
		_rendererDesc = desc.rendererDesc;
	}

	Renderer*	renderer() { return _renderer.ptr(); }

protected:
	virtual void onCreate() override;
	Renderer::CreateDesc	_rendererDesc;

private:
	UPtr<Renderer>			_renderer;
};

} // namespace