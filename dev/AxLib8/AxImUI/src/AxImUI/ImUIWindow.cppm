module;
export module AxImUI:ImUIWindow;
export import :ImUIRenderView;

export namespace AxUI {

class ImUIWindow : public ImUIView {
	AX_RTTI_INFO(ImUIWindow, ImUIView)
public:

	ImUIWindow();
	virtual ~ImUIWindow() override;

	ImUIView*	contentView() { return _contentView.ptr(); }
	
	void setWindowTitle(StrView title);

	void setRenderGraph(RenderGraph* graph);

	RenderContext*	renderContext() { return _contentView ? _contentView->renderContext() : nullptr; }

	void render();

protected:
	virtual void onSetSize(const Vec2f& s) override;
	virtual void onWindowCloseButton() {}

	virtual void onNativeWorldPosChanged(const Vec2f& s);
	virtual void onNativeSizeChanged(const Vec2f& s);

	virtual void onRenderGraph(RenderGraph& graph) {}

	virtual	void onUIMouseEvent(UIMouseEvent& ev) {}
	virtual	void onUIKeyEvent(UIKeyEvent& ev) {}
	
private:
	struct _UIEventHandler : public UIEventHandler {
		_UIEventHandler(ImUIWindow* owner) : _owner(owner) {}
		virtual void onUIMouseEvent(UIMouseEvent& ev) { _owner->onUIMouseEvent(ev); }
		virtual void onUIKeyEvent  (UIKeyEvent&   ev) { _owner->onUIKeyEvent(ev); }
	private:
		ImUIWindow* _owner = nullptr;
	} _uiEventHandler{this};
	
	class NativeWin;
	UPtr<NativeWin>			_nativeWin;
	SPtr<ImUIRenderView>	_contentView;
};

} // namespace
