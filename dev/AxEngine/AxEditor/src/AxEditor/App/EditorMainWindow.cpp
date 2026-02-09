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
	_renderGraph->_owner = this;
	setRenderGraph(_renderGraph);
	setSize({1920, 1080});
}

void EditorMainWindow::onWindowCloseButton() {
	EditorApp::s_instance()->quit(0);
}

void EditorMainWindow::onUIMouseEvent(UIMouseEvent& ev) {
	auto& cam = _renderGraph->_viewportCamera;
	switch (ev.type) {
		case UIMouseEventType::Move: {
			if (ev.pressedButtons == UIMouseEventButton::Right) {
				cam.orbit(ev.deltaPos.yx() * Vec2f(1,-1) * 0.005f);
			} else if (ev.pressedButtons == UIMouseEventButton::Middle) {
				cam.move(ev.deltaPos * 0.02f);
			}
		} break;
		case UIMouseEventType::Wheel: {
			cam.dolly(ev.wheelDelta.y * -0.025f);
		} break;
		default: break;
	}
}

void EditorMainWindow::onUIKeyEvent(UIKeyEvent& ev) {
	if (ev.type == UIKeyEventType::Down) {
		using Op = ImUIGizmoOperation;
		switch (ev.key) {
			case UIKeyCode::Q: _opMode = Op::None;      break;
			case UIKeyCode::W: _opMode = Op::Translate; break;
			case UIKeyCode::E: _opMode = Op::Rotate;    break;
			case UIKeyCode::R: _opMode = Op::Scale;     break;
		}
	}
}

void EditorMainWindow::MyRenderGraph::onBackBufferPass(RenderRequest* req, Span<Input> inputs) {
	Base::onBackBufferPass(req, inputs);
	_owner->_cameraDebugPanel(req);
	_owner->_drawGizmo(req);
	_owner->_sceneOutlinerUIPanel.render(req);
	_owner->_inspectorUIPanel.render(req);
}

void EditorMainWindow::_cameraDebugPanel(RenderRequest* req) {
	ProjectionDesc projDesc = _renderGraph->projectionDesc();
	
	auto& cam = _renderGraph->_viewportCamera;
	ImUIPanel	panel("camera");
	ImUILabelText("viewport"      , Fmt("{}", cam.viewport));
	ImUILabelText("rotation"      , Fmt("{}", cam.rotation.eulerDeg()));
	ImUILabelText("distance"      , Fmt("{}", cam.distance));
	ImUILabelText("eye"           , Fmt("{}", cam.eye()));
	ImUILabelText("aim"           , Fmt("{}", cam.aim));
	ImUILabelText("up"            , Fmt("{}", cam.up()));
	ImUILabelText("viewMatrix"    , Fmt("{}", cam.viewMatrix(projDesc)));
	ImUILabelText("projMatrix"    , Fmt("{}", cam.projMatrix(projDesc)));
	ImUILabelText("viewProjMatrix", Fmt("{}", cam.viewProjMatrix(projDesc)));

	if (ImUIButton(projDesc.isReverseZ ? ZStrView("ReverseZ") : ZStrView("StandardZ"), {160, 40})) {
		AX_TOGGLE_BOOL(projDesc.isReverseZ);
		_renderGraph->setProjectionDesc(projDesc);
	}
	
	if (ImUIButton(projDesc.isRightHanded ? ZStrView("RightHanded") : ZStrView("LeftHanded"), {160, 40})) {
		AX_TOGGLE_BOOL(projDesc.isRightHanded);
		_renderGraph->setProjectionDesc(projDesc);
	}
}

void EditorMainWindow::_drawGizmo(RenderRequest* req) {
	{
		ImUIPanel	panel("Gizmo");
		{
			ImUISameLine();
			if (ImUIRadioButton("Local", _opSpace == ImUIGizmoSpace::Local)) {
				_opSpace = ImUIGizmoSpace::Local;
			}
			
			ImUISameLine();
			if (ImUIRadioButton("World", _opSpace == ImUIGizmoSpace::World)) {
				_opSpace = ImUIGizmoSpace::World;
			}
		}
	}
	
	auto obj = ObjectManager::s_instance()->selection.lastSelectedObject();
	if (!obj) return;
	auto* entity = rttiCast<SceneEntity>(obj.ptr());
	if (!entity) return;

	_gizmoWorldMatrix = entity->worldMatrix();
	_gizmoDeltaMatrix = Mat4f::s_identity();
	
	{
		bool b =  ImUIGizmoIsUsing();
		if (b != _gizmoIsUsing) {
			if (_gizmoIsUsing) {
					
			} else {
				_gizmoStartWorldMatrix = _gizmoWorldMatrix;
			}
		}
		_gizmoIsUsing = b;
	}

	if (ImUIGizmo(	req, _opMode, _opSpace,
					_gizmoWorldMatrix, _gizmoDeltaMatrix,
					_enableTranslateSnap ? &_translateSnap : nullptr)) 
	{
		_gizmoWorldMatrix *= _gizmoDeltaMatrix;
		entity->setWorldMatrix(_gizmoWorldMatrix);
	}
}

} //namespace
