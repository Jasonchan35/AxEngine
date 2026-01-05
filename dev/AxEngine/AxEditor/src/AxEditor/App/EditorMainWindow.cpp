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
	auto& cam = _renderGraph->_camera;
	switch (ev.type) {
		case UIMouseEventType::Move: {
			if (ev.pressedButton == UIMouseEventButton::Right) {
				cam.orbit(ev.deltaPos * 0.001f);
			} else if (ev.pressedButton == UIMouseEventButton::Middle) {
				cam.move(ev.deltaPos * 0.001f);
			}
		} break;
		case UIMouseEventType::Wheel: {
			cam.dolly(ev.wheelDelta.y * -0.05f);
		} break;
	}
	
}

void EditorMainWindow::onUIKeyEvent(UIKeyEvent& ev) {
	
}

void EditorMainWindow::MyRenderGraph::onBackBufferPass(RenderRequest* req, Span<Input> inputs) {
	{
		auto& cam = _camera;
		ImUIPanel	panel("camera");
		ImUILabelText("pos", Fmt("{}", cam.pos()));
		ImUILabelText("aim", Fmt("{}", cam.aim()));
		ImUILabelText("up",  Fmt("{}", cam.up()));
		auto viewMat = cam.viewMatrix();
		ImUILabelText("viewMatrix",  Fmt("{}", viewMat));
		auto projMat = cam.projMatrix();
		ImUILabelText("projMatrix",  Fmt("{}", projMat));
		auto viewProjMat = cam.viewProjMatrix();
		ImUILabelText("viewProjMatrix",  Fmt("{}", viewProjMat));
	}

	win->_sceneOutlinerUIPanel.render(req);
	DefaultRenderGraph::onBackBufferPass(req, inputs);
}

} //namespace
