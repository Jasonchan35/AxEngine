module;
export module AxUI:UIWindow;
export import :UIRenderView;

export namespace ax::AxUI {

class UIWindow : public UIView {
	AX_RTTI_INFO(UIWindow, UIView)
public:

	UIWindow();
	virtual ~UIWindow() override;

	UIView*	contentView() { return _contentView.ptr(); }

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

private:
	class NativeWin;
	UPtr<NativeWin>			_nativeWin;
	SPtr<UIRenderView>		_contentView;
};

} // namespace
