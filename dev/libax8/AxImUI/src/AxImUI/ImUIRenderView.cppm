module;
export module AxImUI:ImUIRenderView;
export import :ImUIView;

export namespace AxUI {

class ImUIRenderView : public ImUIView {
	AX_RTTI_INFO(ImUIRenderView, ImUIView)
public:
	void createRenderContext(RenderContext::CreateDesc& desc);

	RenderContext*	renderContext() { return _renderContext; }

	void render();

protected:
	virtual void onSizeChanged(const Vec2f& size) override;

private:
	UPtr<RenderContext>	_renderContext;
};

} // namespace