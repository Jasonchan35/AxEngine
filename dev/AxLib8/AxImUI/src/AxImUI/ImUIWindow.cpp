module AxImUI;
import :ImUIWindow;

namespace AxUI {

class ImUIWindow::NativeWin : public NativeUIWindow {
	AX_RTTI_INFO(NativeWin, NativeUIWindow)
public:
	NativeWin(ImUIWindow* owner, CreateDesc& desc) 
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
	
	virtual	void onNativeUIMouseEvent(NativeUIMouseEvent& ev) override {
		_owner->onNativeUIMouseEvent(ev);
	}
	virtual	void onNativeUIKeyEvent(NativeUIKeyEvent& ev) override {
		_owner->onNativeUIKeyEvent(ev);
	}
	

private:
	ImUIWindow* _owner = nullptr;
};

ImUIWindow::ImUIWindow() {
	{
		NativeUIWindow_CreateDesc desc;
		desc.isMainWindow = true;
		desc.rect = Rect2f(100, 100, 1920, 1080) / 2;
	//	desc.visible = false;
		_nativeWin = UPtr_new<NativeWin>(AX_NEW, this, desc);
	}
	
	{
		_contentView = SPtr_new<ImUIRenderView>(AX_NEW);
		addChild(_contentView);

		RenderContext::CreateDesc desc;
		desc.window = _nativeWin.ptr();
		_contentView->createRenderContext(desc);
	}

	auto* renderSystem = RenderSystem::s_instance();
	auto title = Fmt("Test - {}, MT: {}, VSync: {}", renderSystem->api(), renderSystem->multithread(), renderSystem->vsync());
	setWindowTitle(title);
//	setWindowActive(true);

	setSize({1280, 768});
}

void ImUIWindow::onSetSize(const Vec2f& s) {
	Base::onSetSize(s);

	if (_nativeWin) {
		_nativeWin->setSize(s);
	}
}

void ImUIWindow::onNativeWorldPosChanged(const Vec2f& s) {
	Base::onWorldPosChanged(s);
}

void ImUIWindow::onNativeSizeChanged(const Vec2f& s) {
	Base::onSizeChanged(s);

	if (_nativeWin) {
		auto rc = _nativeWin->contentWorldRect();
		if (auto v = contentView()) {
			v->setWorldRect(rc);
		}
	}
}

ImUIWindow::~ImUIWindow() {
}

void ImUIWindow::setWindowTitle(StrView title) { _nativeWin->setWindowTitle(title); }

void ImUIWindow::setRenderGraph(RenderGraph* graph) {
	if (auto* ctx = renderContext())
		ctx->setRenderGraph(graph);
}

void ImUIWindow::render() {
	if (_contentView) {
		_contentView->render();
	}
}

} // namespace 