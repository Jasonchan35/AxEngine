module AxUI;
import :UIWindow;

namespace ax::AxUI {

class UIWindow::NativeWin : public NativeUIWindow {
	AX_RTTI_INFO(NativeWin, NativeUIWindow)
public:
	NativeWin(UIWindow* owner, CreateDesc& desc) 
	: Base(desc)
	, _owner(owner)
	{
	}

	virtual void onCloseButton() override {
		_owner->onWindowCloseButton();
	}

	virtual void onWorldPosChanged(const Vec2f& pos) override {
		Base::onWorldPosChanged(pos);
		_owner->onNativeWorldPosChanged(pos);
	}

	virtual void onSizeChanged(const Vec2f& size) override {
		Base::onSizeChanged(size);
		_owner->onNativeSizeChanged(size);
	}

private:
	UIWindow* _owner = nullptr;
};

UIWindow::UIWindow() {
	TempString title("Test");

	{
		NativeUIWindow_CreateDesc desc;
		desc.isMainWindow = true;
	//	desc.visible = false;
		_nativeWin = UPtr_new<NativeWin>(AX_ALLOC_REQ, this, desc);
	}
	
	{
		_contentView = SPtr_new<UIRenderView>(AX_ALLOC_REQ);
		addChild(_contentView);

		RenderContext::CreateDesc desc;
		desc.window = _nativeWin.ptr();
		_contentView->createRenderContext(desc);
	}

//	auto* app = EditorApp::s_instance();
//	auto* renderer = app->renderer();
//	FmtTo(title, " - {}, MT: {}, VSync: {}", renderer->api(), renderer->multithread(), renderer->vsync());
//	setWindowTitle(title);
//	setWindowActive(true);

	setSize({1280, 768});
}

void UIWindow::onSetSize(const Vec2f& s) {
	Base::onSetSize(s);

	if (_nativeWin) {
		_nativeWin->setSize(s);
	}
}

void UIWindow::onNativeWorldPosChanged(const Vec2f& s) {
	Base::onWorldPosChanged(s);
}

void UIWindow::onNativeSizeChanged(const Vec2f& s) {
	Base::onSizeChanged(s);

	if (_nativeWin) {
		auto rc = _nativeWin->contentWorldRect();
		if (auto v = contentView()) {
			v->setWorldRect(rc);
		}
	}
}

UIWindow::~UIWindow() {
}

void UIWindow::setWindowTitle(StrView title) { _nativeWin->setWindowTitle(title); }

void UIWindow::setRenderGraph(RenderGraph* graph) {
	if (auto* ctx = renderContext())
		ctx->setRenderGraph(graph);
}

void UIWindow::render() {
	if (_contentView) {
		_contentView->render();
	}
}

} // namespace 