module;

module AxEditor;
import :EditorMainWindow;
import :EditorApp;

namespace AxEditor {

EditorMainWindow::EditorMainWindow() {
	_gizmoOp = ImUIGizmoOperation::Translate;
	_gpuDebugData = {};
	_gpuDebugData.debugColorCode = AxGpuDebugColorCode_Tri;
//	_gpuDebugData.drawNormalLength = 0.25f;
//	_gpuDebugData.flags = ax_bit_set(_gpuDebugData.flags, AxGpuData_Debug_FLAG_DisableFrustumCulling);
	
	_flyingCameraSpeed = 20.0f;
	
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
	if (ev.hasAtlKey()) {
		_inputMode = InputMode::Camera;
	} else if (ev.pressedButtons == UIMouseEventButton::Right || ev.pressedButtons == UIMouseEventButton::Middle) {
		_inputMode = InputMode::FlyingCamera;
	} else {
		_inputMode = InputMode::None;
	}
	
	auto& cam = _renderGraph->viewportCamera();
	switch (ev.type) {
		case UIMouseEventType::Move: {
			if (_inputMode == InputMode::Camera) {
				if (ev.pressedButtons == UIMouseEventButton::Left) {
					cam.orbit(ev.deltaPos.yx() * Vec2f(1,-1) * 0.15f);
					
				} else if (ev.pressedButtons == UIMouseEventButton::Middle) {
					cam.move(ev.deltaPos * 0.01f * _mouseSpeed);
					
				} else if (ev.pressedButtons == UIMouseEventButton::Right) {
					cam.dolly(ev.deltaPos.y * 0.015f * _mouseSpeed);
				}
			} else if (_inputMode == InputMode::FlyingCamera) {
				if (ev.pressedButtons == UIMouseEventButton::Right) {
					cam.pan(ev.deltaPos.yx() * Vec2f(1,-1) * 0.15f);
					
				} else if (ev.pressedButtons == UIMouseEventButton::Middle) {
					cam.move(ev.deltaPos * 0.01f * _mouseSpeed);
				}
			}
		} break;
			
		case UIMouseEventType::Wheel: {
			cam.dolly(ev.wheelDelta.y * -0.015f * _mouseSpeed);
		} break;
		default: break;
	}
}

void EditorMainWindow::onUIKeyEvent(UIKeyEvent& ev) {
	if (_inputMode == InputMode::None) {
		if (ev.type == UIKeyEventType::Down) {
			using Op = ImUIGizmoOperation;
			switch (ev.key) {
				case UIKeyCode::Q: _gizmoOp = Op::None;      break;
				case UIKeyCode::W: _gizmoOp = Op::Translate; break;
				case UIKeyCode::E: _gizmoOp = Op::Rotate;    break;
				case UIKeyCode::R: {
					_gizmoOp = _gizmoOp == Op::Scale ? Op::Bounds : Op::Scale;
				} break;
				case UIKeyCode::T: _gizmoOp = Op::Universal; break;
				default: break;
			}
		}
	}
}

void EditorMainWindow::MyRenderGraph::onUpdate(RenderRequest* req) {
	Base::onUpdate(req);
}

void EditorMainWindow::MyRenderGraph::onBackBufferPass(RenderRequest* req, Span<Input> inputs) {
	Base::onBackBufferPass(req, inputs);
	
	ImUIGetWindowDrawList();
	
	_owner->_drawGizmo(req);
	_owner->_viewportCameraPanel(req);
	_owner->_sceneOutlinerUIPanel.render(Engine::s_instance()->world(), req);
	_owner->_inspectorUIPanel.render(req);
	_owner->_statisticsPanel(req);
}

void EditorMainWindow::_statisticsPanel(RenderRequest* req) {
	ImUIPanel	panel("statistics");
	
	_fpsCount.update(req->deltaTime());
	ImUIText(Fmt("Frame: {:<8} Uptime: {:<3.2f}\nFPS: {:<8.2f} ({:>8.3f}ms)",
	             req->renderSeqId(), req->uptime(),
	             _fpsCount.fps(), _fpsCount.averageTime * 1000));
	
	ImUIText("Meshlet:");
	auto func = [](StrView name, const GpuBufferPool::Statistics& src)-> void {
		ImUIText(Fmt("  {} ({:8} / {:2} / {:4}MB)",
		             name,
		             src.elementCount,
		             src.allocationCount,
		             src.sizeInBytes / Math::MegaBytes));
	};
	
	auto& stat = req->statistics;
	func("Cluster", stat.meshletCluster);
	func("Group  ", stat.meshletGroup);
	func("Vert   ", stat.meshletVert);
	func("Prim   ", stat.meshletPrim);
}

void EditorMainWindow::FpsCount::update(double deltaTime) {
	_time += deltaTime;
	_frames++;
	if (_time > 0.5) {
		averageTime = _time / static_cast<double>(_frames);
		_time   = 0;
		_frames = 0;
	}
}

void EditorMainWindow::_viewportCameraPanel(RenderRequest* req) {
	ProjectionDesc projDesc = _renderGraph->projectionDesc();
	
	auto& cam = _renderGraph->viewportCamera();
	
	_flyingCameraMoveVector = Vec3f::s_zero();
	
	if (_inputMode == InputMode::FlyingCamera) {
		
		if (NativeUIApp::s_getAsyncKeyState(UIKeyCode::W)) { _flyingCameraMoveVector.z += 1; }
		if (NativeUIApp::s_getAsyncKeyState(UIKeyCode::S)) { _flyingCameraMoveVector.z -= 1; }
		if (NativeUIApp::s_getAsyncKeyState(UIKeyCode::A)) { _flyingCameraMoveVector.x += 1; }
		if (NativeUIApp::s_getAsyncKeyState(UIKeyCode::D)) { _flyingCameraMoveVector.x -= 1; }
		if (NativeUIApp::s_getAsyncKeyState(UIKeyCode::E)) { _flyingCameraMoveVector.y += 0.5f; }
		if (NativeUIApp::s_getAsyncKeyState(UIKeyCode::Q)) { _flyingCameraMoveVector.y -= 0.5f; }

		cam.move(_flyingCameraMoveVector * (req->deltaTime() * _flyingCameraSpeed));
	}	
	
	ImUIPanel	panel("camera");
	
	ImUIDragFloat("fieldOfView"   , cam.fieldOfView, 0.1f, 5, 180);
	ImUIDragEuler("rotation"      , cam.rotation);
	ImUIDragFloat("distance"      , cam.distance);
	
	if (ImUIButton(projDesc.isReverseZ ? ZStrView("ReverseZ") : ZStrView("StandardZ"), {160, 40})) {
		AX_TOGGLE_BOOL(projDesc.isReverseZ);
		_renderGraph->setProjectionDesc(projDesc);
	}
	
	ImUISameLine();
	if (ImUIButton(projDesc.isRightHanded ? ZStrView("RightHanded") : ZStrView("LeftHanded"), {160, 40})) {
		AX_TOGGLE_BOOL(projDesc.isRightHanded);
		_renderGraph->setProjectionDesc(projDesc);
	}
	
	ImUILabelText("viewport"      , Fmt("{}", cam.viewport));
	ImUILabelText("eye"           , Fmt("{}", cam.eye()));
	ImUILabelText("aim"           , Fmt("{}", cam.aim));
	ImUILabelText("up"            , Fmt("{}", cam.up()));
	
//	ImUILabelText("worldMatrix"   , Fmt("{}", cam.worldMatrix(projDesc)));
//	ImUILabelText("worldMatrixInv", Fmt("{}", cam.worldMatrix(projDesc).inverse()));
//	ImUILabelText("viewMatrix (lookAt)", Fmt("{}", cam.viewMatrix(projDesc)));
	
	auto& camData = req->cameraData();
	ImUILabelText("viewMatrix"   , Fmt("{}", camData.viewMatrix));
	ImUILabelText("projMatrix"   , Fmt("{}", camData.projMatrix));
	
	ImUIText("CullingPlane");
	ImUILabelText("[0] near  ", Fmt("{}", camData.cullingPlanes[0]));
	ImUILabelText("[1] far   ", Fmt("{}", camData.cullingPlanes[1]));
	ImUILabelText("[2] left  ", Fmt("{}", camData.cullingPlanes[2]));
	ImUILabelText("[3] right ", Fmt("{}", camData.cullingPlanes[3]));
	ImUILabelText("[4] top   ", Fmt("{}", camData.cullingPlanes[4]));
	ImUILabelText("[5] bottom", Fmt("{}", camData.cullingPlanes[5]));
}

void EditorMainWindow::_drawGizmo(RenderRequest* req) {
	auto& cam = req->cameraData();
	
	ImUIDrawGizmoRequest gizmoRequest;
	gizmoRequest.viewMatrix = cam.viewMatrix;
	gizmoRequest.projMatrix = cam.projMatrix;
	
	float ViewManipulateOffset = 20;
	float ViewManipulateSize = 120;
	
	ImUIGizmoViewManipulate(&gizmoRequest,
	                        Rect2f(req->viewport().w - ViewManipulateSize - ViewManipulateOffset,
	                               ViewManipulateOffset,
	                               ViewManipulateSize,
	                               ViewManipulateSize));
	
	{
		ImUIPanel	panel("Gizmo");
		ImUIDragFloat("mouseSpeed", _mouseSpeed, 0.1f, 0.1f, 50.0f);
		ImUIDragFloat("flyingCameraSpeed", _flyingCameraSpeed, 0.1f, 0.1f, 500.0f);
		
		{
			if (ImUIRadioButton("Local", _gizmoSpace == ImUIGizmoSpace::Local)) {
				_gizmoSpace = ImUIGizmoSpace::Local;
			}
			
			ImUISameLine();
			if (ImUIRadioButton("World", _gizmoSpace == ImUIGizmoSpace::World)) {
				_gizmoSpace = ImUIGizmoSpace::World;
			}
		}
		
		ImUICheckBox("useCullingCamera", _useCullingCamera);
		ImUISameLine();
		if (ImUIButton("Align to View", {160, 25})) {
			if (_cullingCameraComp) {
				_cullingCameraComp->entity()->setWorldMatrix(_renderGraph->viewportCamera().worldMatrix());
			}
		}

		ImUIDragFloat("maxMeshletErrorInPixels", _maxMeshletErrorInPixels, 0.1f, 0, 20);
		req->maxMeshletErrorInPixels = _maxMeshletErrorInPixels;
		{
			ImUICheckBoxArray_Item<i32> list_[] = {
				{.name = "None"    , .value = AxGpuDebugColorCode_None               },
				{.name = "Tri"     , .value = AxGpuDebugColorCode_Tri                },
				{.name = "Cluster" , .value = AxGpuDebugColorCode_MeshletCluster     },
				{.name = "Group"   , .value = AxGpuDebugColorCode_MeshletGroup       },
				{.name = "Refine"  , .value = AxGpuDebugColorCode_MeshletRefinedGroup},
				{.name = "Lod"     , .value = AxGpuDebugColorCode_MeshletLod         },
			};
			auto list = Span(list_);
			ImUICheckBoxArray("debugColorCode", _gpuDebugData.debugColorCode, list);
			
			if (_gpuDebugData.debugColorCode == AxGpuDebugColorCode_MeshletLod) {
				ImUIText("LoD-0"); ImUISameLine(); ImUIColorButton("LoD-0", Color4f(1,0,0,1));
				ImUIText("LoD-1"); ImUISameLine(); ImUIColorButton("LoD-1", Color4f(0,1,0,1));
				ImUIText("LoD-2"); ImUISameLine(); ImUIColorButton("LoD-2", Color4f(1,1,0,1));
				ImUIText("LoD-3"); ImUISameLine(); ImUIColorButton("LoD-3", Color4f(0,0,1,1));
				ImUIText("LoD-4"); ImUISameLine(); ImUIColorButton("LoD-4", Color4f(1,0,1,1));
				ImUIText("LoD-5"); ImUISameLine(); ImUIColorButton("LoD-5", Color4f(0,1,1,1));
			}
		}
		
		ImUIDragFloat("showAllLodDistance", _gpuDebugData.showAllLodDistance, 0.1f, 0, 10);
		ImUIDragFloat("Normal Length",      _gpuDebugData.drawNormalLength, 0.01f, 0, 4);
		ImUICheckBoxFlag("Disable Frustum Culling", _gpuDebugData.flags, AxGpuData_Debug_FLAG_DisableFrustumCulling);
	}

	req->setDebugData(_gpuDebugData);

	if (_useCullingCamera && _cullingCameraComp) {
		ImUIGizmoCamera(&gizmoRequest,
						_cullingCameraComp->cameraObj->camera,
						_cullingCameraComp->entity()->worldMatrix(),
						req->projectionDesc());
		
		req->setCullingCamera(_cullingCameraComp->cameraObj->camera, _cullingCameraComp->entity()->worldMatrix());
	}

	auto selectedObject = ObjectManager::s_instance()->selection.lastSelectedObject();
	if (!selectedObject) return;
	auto* selectdEntity = rttiCast<SceneEntity>(selectedObject.ptr());
	if (!selectdEntity) return;

	Mat4f worldMatrix = selectdEntity->worldMatrix();

	{
		bool b =  ImUIGizmoIsUsing();
		if (b != _gizmoIsUsing) {
			if (!_gizmoIsUsing) {
				_gizmoStartWorldMatrix = worldMatrix;
			}
		}
		_gizmoIsUsing = b;
	}
	
	Vec3f* snap = nullptr;
	switch (_gizmoOp) {
		case ImUIGizmoOperation::Translate: if (_enableTranslateSnap) { snap = &_translateSnap; } break;
		case ImUIGizmoOperation::Rotate   : if (_enableRotateSnap   ) { snap = &_rotateSnap;    } break;
		case ImUIGizmoOperation::Scale    : if (_enableScaleSnap    ) { snap = &_scaleSnap;     } break;
		default: break;
	}

	BBox3f bounds = BBox3f::s_empty();
	if (auto* meshRenderer = selectdEntity->getComponent<MeshRendererComponent>()) {
		if (auto* mesh = meshRenderer->mesh.ptr()) {
			bounds = mesh->bounds();
		}
	}

	if (ImUIGizmoManipulate(&gizmoRequest, _gizmoOp, _gizmoSpace, snap, bounds, worldMatrix)) {
		selectdEntity->setWorldMatrix(worldMatrix);
	}
	
	if (auto* comp = selectdEntity->getComponent<CameraComponent>()) {
		ImUIGizmoCamera(&gizmoRequest,
		                comp->cameraObj->camera,
		                selectdEntity->worldMatrix(),
		                req->projectionDesc());
	}
}

} //namespace
