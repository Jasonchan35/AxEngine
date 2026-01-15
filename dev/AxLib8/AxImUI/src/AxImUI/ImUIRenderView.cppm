module;
export module AxImUI:ImUIRenderView;
export import :ImUIView;

export namespace ax {

class ImUIRenderView : public ImUIView {
	AX_RTTI_INFO(ImUIRenderView, ImUIView)
public:
	void createRenderContext(RenderContext::CreateDesc& desc);

	RenderContext*	renderContext() { return _renderContext; }

	void render();
	
	void setUIEventHandler(UIEventHandler* uiEventHandler) {
		_renderContext->setUIEventHandler(uiEventHandler);
	} 

protected:
	virtual void onSizeChanged(const Vec2f& size) override;

private:
	UPtr<RenderContext>	_renderContext;
};

} // namespace