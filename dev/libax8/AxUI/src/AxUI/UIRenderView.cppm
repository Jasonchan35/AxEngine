module;
export module AxUI:UIRenderView;
export import :UIView;

export namespace AxUI {

class UIRenderView : public UIView {
	AX_RTTI_INFO(UIRenderView, UIView)
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