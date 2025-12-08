module AxEditor.EditorApp;

namespace ax::AxEditor {

EditorMainWindow::EditorMainWindow() {
	setWindowTitle("AnonEditor");

	if (auto* rc = renderContext()) {
		rc->setImGuiIniFilename(".imgui_EditorMainWindow.ini");
		rc->loadImGuiIniFile();
	}

	_renderGraph.newObject(AX_ALLOC_REQ);
	_renderGraph->win = this;
	setRenderGraph(_renderGraph);
}

void EditorMainWindow::onWindowCloseButton() {
	EditorApp::s_instance()->quit(0);
}

} // namespace 