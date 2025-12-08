module;
export module AxUI:UIApp;
export import :PCH;

export namespace ax::AxUI {

class UIApp : public NativeUIApp {
	AX_RTTI_INFO(UIApp, NativeUIApp)
public:
	AX_DOWNCAST_GET_INSTANCE()

	UIApp(const CreateDesc& desc) : Base(desc) {}

	Renderer*	renderer() { return _renderer.ptr(); }

protected:
	virtual void onCreate() override;
	Renderer::CreateDesc	_rendererCreateDesc;

private:
	UPtr<Renderer>			_renderer;
};

} // namespace