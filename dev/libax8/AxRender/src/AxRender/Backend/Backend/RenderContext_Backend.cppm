module;
export module AxRender:RenderContext_Backend;
export import :RenderContext;
export import :AxImGui;

export namespace ax::AxRender {

class RenderPass_Backend;

class RenderContext_Backend : public RenderContext {
	AX_RTTI_INFO(RenderContext_Backend, RenderContext)
public:
	RenderContext_Backend(const CreateDesc& desc);
	virtual ~RenderContext_Backend() override;

	virtual void onRender() override;

	virtual RenderPass_Backend* onAcquireBackBufferRenderPass(RenderRequest* req) = 0;
	virtual void onPresentSurface(RenderRequest* req) = 0;

	virtual void onPostCreate(const CreateDesc& desc) override;
	virtual void onUIMouseEvent(NativeUIMouseEvent& ev) override;
	virtual void onUIKeyEvent(NativeUIKeyEvent& ev) override;

	virtual void onSetImGuiIniFilename(StrView name) override;
	virtual void onLoadImGuiIniFile() override;
	virtual void onSaveImGuiIniFile() override;

	AxImGui	imgui;
};

} // namespace