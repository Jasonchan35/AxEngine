module;

module AxEditor;
import :EditorMainWindow;
import :EditorApp;

namespace AxEditor {

EditorMainWindow::EditorMainWindow() {
	_gizmoOp = ImGizmo_ManipulateType::Translate;
	
//	_useCullingCamera = true;
	
	_gpuDebugData = {};
	_gpuDebugData.debugColorCode = AxGpuDebugColorCode_Tri;
//	_gpuDebugData.drawCluster = 1.0f;
//	_gpuDebugData.drawNormalLength = 0.25f;
//	_gpuDebugData.flags = ax_bit_set(_gpuDebugData.flags, AxGpuData_Debug_FLAG_DisableFrustumCulling);
	_gpuDebugData.flags = ax_bit_set(_gpuDebugData.flags, AxGpuData_Debug_FLAG_DisableBackConeCulling);
	
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
			using Op = ImGizmo_ManipulateType;
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
	
	ImUI_GetWindowDrawList();
	
	_owner->_drawGizmo(req);
	_owner->_viewportCameraPanel(req);
	_owner->_sceneOutlinerUIPanel.render(Engine::s_instance()->world(), req);
	_owner->_inspectorUIPanel.render(req);
	_owner->_statisticsPanel(req);
}

void EditorMainWindow::_statisticsPanel(RenderRequest* req) {
	ImUI_Panel	panel("statistics");
	
	_fpsCount.update(req->deltaTime());
	ImUI_Text(Fmt("Frame: {:<8} Uptime: {:<3.2f}\nFPS: {:<8.2f} ({:>8.3f}ms)",
	             req->renderSeqId(), req->uptime(),
	             _fpsCount.fps(), _fpsCount.averageTime * 1000));
	
	ImUI_Text("Meshlet:");
	auto func = [](StrView name, const GpuBufferPool::Statistics& src)-> void {
		ImUI_Text(Fmt("  {} ({:8} / {:2} / {:4}MB)",
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
	
	ImUI_Panel	panel("camera");
	
	ImUI_DragFloat("fieldOfView"   , cam.fieldOfView, 0.1f, 5, 180);
	ImUI_DragEuler("rotation"      , cam.rotation);
	ImUI_DragFloat("distance"      , cam.distance);
	
	if (ImUI_Button(projDesc.isReverseZ ? ZStrView("ReverseZ") : ZStrView("StandardZ"), {160, 40})) {
		AX_TOGGLE_BOOL(projDesc.isReverseZ);
		_renderGraph->setProjectionDesc(projDesc);
	}
	
	ImUI_SameLine();
	if (ImUI_Button(projDesc.isRightHanded ? ZStrView("RightHanded") : ZStrView("LeftHanded"), {160, 40})) {
		AX_TOGGLE_BOOL(projDesc.isRightHanded);
		_renderGraph->setProjectionDesc(projDesc);
	}
	
	ImUI_LabelText("viewport"      , Fmt("{}", cam.viewport));
	ImUI_LabelText("eye"           , Fmt("{}", cam.eye()));
	ImUI_LabelText("aim"           , Fmt("{}", cam.aim));
	ImUI_LabelText("up"            , Fmt("{}", cam.up()));
	
//	ImUILabelText("worldMatrix"   , Fmt("{}", cam.worldMatrix(projDesc)));
//	ImUILabelText("worldMatrixInv", Fmt("{}", cam.worldMatrix(projDesc).inverse()));
//	ImUILabelText("viewMatrix (lookAt)", Fmt("{}", cam.viewMatrix(projDesc)));
	
	auto& camData = req->cameraData();
	ImUI_LabelText("viewMatrix"   , Fmt("{}", camData.viewMatrix));
	ImUI_LabelText("projMatrix"   , Fmt("{}", camData.projMatrix));
	
	ImUI_Text("CullingPlane");
	ImUI_LabelText("[0] near  ", Fmt("{}", camData.cullingPlanes[0]));
	ImUI_LabelText("[1] far   ", Fmt("{}", camData.cullingPlanes[1]));
	ImUI_LabelText("[2] left  ", Fmt("{}", camData.cullingPlanes[2]));
	ImUI_LabelText("[3] right ", Fmt("{}", camData.cullingPlanes[3]));
	ImUI_LabelText("[4] top   ", Fmt("{}", camData.cullingPlanes[4]));
	ImUI_LabelText("[5] bottom", Fmt("{}", camData.cullingPlanes[5]));
}

void EditorMainWindow::_drawGizmo(RenderRequest* req) {
	auto& cam = req->cameraData();
	
	ImGizmo_DrawRequest gizmoRequest;
	gizmoRequest.renderRequest = req;
	gizmoRequest.viewport   = req->viewport();
	gizmoRequest.viewMatrix = cam.viewMatrix;
	gizmoRequest.projMatrix = cam.projMatrix;
	
	float ViewManipulateOffset = 20;
	float ViewManipulateSize = 120;
	
	ImGizmo_ViewManipulate(&gizmoRequest,
	                        Rect2f(req->viewport().w - ViewManipulateSize - ViewManipulateOffset,
	                               ViewManipulateOffset,
	                               ViewManipulateSize,
	                               ViewManipulateSize));
	
	{
		ImUI_Panel	panel("Gizmo");
		ImUI_DragFloat("mouseSpeed", _mouseSpeed, 0.1f, 0.1f, 50.0f);
		ImUI_DragFloat("flyingCameraSpeed", _flyingCameraSpeed, 0.1f, 0.1f, 500.0f);
		
		{
			if (ImUI_RadioButton("Local", _gizmoSpace == ImGizmo_Space::Local)) {
				_gizmoSpace = ImGizmo_Space::Local;
			}
			
			ImUI_SameLine();
			if (ImUI_RadioButton("World", _gizmoSpace == ImGizmo_Space::World)) {
				_gizmoSpace = ImGizmo_Space::World;
			}
		}
		
		ImUI_CheckBox("useCullingCamera", _useCullingCamera);
		ImUI_SameLine();
		if (ImUI_Button("Align to View", {160, 25})) {
			if (_cullingCameraComp) {
				_cullingCameraComp->entity()->setWorldMatrix(_renderGraph->viewportCamera().worldMatrix());
			}
		}

		ImUI_DragFloat("maxMeshletErrorInPixels", _maxMeshletErrorInPixels, 0.1f, 0, 20);
		req->maxMeshletErrorInPixels = _maxMeshletErrorInPixels;
		{
			ImUI_CheckBoxArray_Item<i32> list_[] = {
				{.name = "None"    , .value = AxGpuDebugColorCode_None               },
				{.name = "Tri"     , .value = AxGpuDebugColorCode_Tri                },
				{.name = "Cluster" , .value = AxGpuDebugColorCode_MeshletCluster     },
				{.name = "Group"   , .value = AxGpuDebugColorCode_MeshletGroup       },
				{.name = "Refine"  , .value = AxGpuDebugColorCode_MeshletRefinedGroup},
				{.name = "Lod"     , .value = AxGpuDebugColorCode_MeshletLod         },
			};
			auto list = Span(list_);
			ImUI_CheckBoxArray("debugColorCode", _gpuDebugData.debugColorCode, list);
			
			if (_gpuDebugData.debugColorCode == AxGpuDebugColorCode_MeshletLod) {
				ImUI_Text("LoD-0"); ImUI_SameLine(); ImUI_ColorButton("LoD-0", Color4f(1,0,0,1));
				ImUI_Text("LoD-1"); ImUI_SameLine(); ImUI_ColorButton("LoD-1", Color4f(0,1,0,1));
				ImUI_Text("LoD-2"); ImUI_SameLine(); ImUI_ColorButton("LoD-2", Color4f(1,1,0,1));
				ImUI_Text("LoD-3"); ImUI_SameLine(); ImUI_ColorButton("LoD-3", Color4f(0,0,1,1));
				ImUI_Text("LoD-4"); ImUI_SameLine(); ImUI_ColorButton("LoD-4", Color4f(1,0,1,1));
				ImUI_Text("LoD-5"); ImUI_SameLine(); ImUI_ColorButton("LoD-5", Color4f(0,1,1,1));
			}
		}
		
		ImUI_DragFloat("showAllLodDistance" , _gpuDebugData.showAllLodDistance , 0.1f, 0, 10);
		ImUI_DragFloat("Debug Normal Length", _gpuDebugData.drawNormalLength   , 0.01f, 0, 4);
		ImUI_DragFloat("Debug Cluster"      , _gpuDebugData.drawCluster        , 0.01f, 0, 4);
		ImUI_CheckBoxFlag("Disable Frustum  Culling", _gpuDebugData.flags, AxGpuData_Debug_FLAG_DisableFrustumCulling);
		ImUI_CheckBoxFlag("Disable BackCone Culling", _gpuDebugData.flags, AxGpuData_Debug_FLAG_DisableBackConeCulling);
	}

	req->setDebugData(_gpuDebugData);

	if (_useCullingCamera && _cullingCameraComp) {
		ImGizmo_Camera(&gizmoRequest,
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
		bool b =  ImGizmo_IsUsing();
		if (b != _gizmoIsUsing) {
			if (!_gizmoIsUsing) {
				_gizmoStartWorldMatrix = worldMatrix;
			}
		}
		_gizmoIsUsing = b;
	}
	
	Vec3f* snap = nullptr;
	switch (_gizmoOp) {
		case ImGizmo_ManipulateType::Translate: if (_enableTranslateSnap) { snap = &_translateSnap; } break;
		case ImGizmo_ManipulateType::Rotate   : if (_enableRotateSnap   ) { snap = &_rotateSnap;    } break;
		case ImGizmo_ManipulateType::Scale    : if (_enableScaleSnap    ) { snap = &_scaleSnap;     } break;
		default: break;
	}

	for (auto& comp : selectdEntity->components()) {
		comp->onDrawGizmo(&gizmoRequest);
	}
	
	BBox3f bounds = BBox3f::s_empty();
	if (auto* meshRenderer = selectdEntity->getComponent<MeshRendererComponent>()) {
		if (auto* mesh = meshRenderer->mesh.ptr()) {
			bounds = mesh->bounds();
		}
	}

	if (ImGizmo_Manipulate(&gizmoRequest, _gizmoOp, _gizmoSpace, snap, bounds, worldMatrix)) {
		selectdEntity->setWorldMatrix(worldMatrix);
	}
}

} //namespace
