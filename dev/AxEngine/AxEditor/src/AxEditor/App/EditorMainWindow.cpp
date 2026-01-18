module;

module AxEditor;
import :EditorMainWindow;
import :EditorApp;

namespace AxEditor {

EditorMainWindow::EditorMainWindow() {
	auto* renderSystem = RenderSystem::s_instance();
	auto  title    = Fmt("AxEditor - {}{}, MT: {}, VSync: {}",
	                     renderSystem->api(),
	                     AxRenderConfig::bindless ? "-bindless" : "",
	                     renderSystem->multithread(),
	                     renderSystem->vsync());

	setWindowTitle(title);

	if (auto* rc = renderContext()) {
		rc->setImGuiIniFilename(".imgui_EditorMainWindow.ini");
		rc->loadImGuiIniFile();
	}

	_renderGraph.newObject(AX_NEW);
	_renderGraph->win = this;
	setRenderGraph(_renderGraph);
}

void EditorMainWindow::onWindowCloseButton() {
	EditorApp::s_instance()->quit(0);
}

void EditorMainWindow::onUIMouseEvent(UIMouseEvent& ev) {
	auto& cam = _renderGraph->_viewportCamera;
	switch (ev.type) {
		case UIMouseEventType::Move: {
			if (ev.pressedButtons == UIMouseEventButton::Right) {
				cam.orbit(ev.deltaPos.yx() * -0.005f);
			} else if (ev.pressedButtons == UIMouseEventButton::Middle) {
				cam.move(ev.deltaPos * 0.02f * Vec2f(1,-1));
			}
		} break;
		case UIMouseEventType::Wheel: {
			cam.dolly(ev.wheelDelta.y * -0.05f);
		} break;
		default: break;
	}
}

void EditorMainWindow::onUIKeyEvent(UIKeyEvent& ev) {
	
}

void EditorMainWindow::MyRenderGraph::onBackBufferPass(RenderRequest* req, Span<Input> inputs) {
	Base::onBackBufferPass(req, inputs);
	
	if constexpr (true) {
		auto& cam = _viewportCamera;
		ImUIPanel	panel("camera");
		ImUILabelText("viewport"      , Fmt("{}", cam.viewport()));
		ImUILabelText("pos"           , Fmt("{}", cam.pos()));
		ImUILabelText("aim"           , Fmt("{}", cam.aim()));
		ImUILabelText("up"            , Fmt("{}", cam.up()));
		ImUILabelText("Rot"           , Fmt("{}", cam.rotation()));
		ImUILabelText("viewMatrix"    , Fmt("{}", cam.viewMatrix()));
		ImUILabelText("projMatrix"    , Fmt("{}", cam.projMatrix()));
		ImUILabelText("viewProjMatrix", Fmt("{}", cam.viewProjMatrix()));
	}

	win->_sceneOutlinerUIPanel.render(req);
	win->_inspectorUIPanel.render(req);
}

} //namespace
