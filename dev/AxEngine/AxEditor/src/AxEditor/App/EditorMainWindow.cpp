module;

module AxEditor;
import :EditorMainWindow;
import :EditorApp;

namespace AxEditor {

EditorMainWindow::EditorMainWindow() {
	_gpuDebugData = {};
	_gpuDebugData.debugColorCode = AxGpuDebugColorCode_Tri;
//	_gpuDebugData.drawNormalLength = 0.25f;
	
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
	auto& cam = _renderGraph->viewportCamera();
	switch (ev.type) {
		case UIMouseEventType::Move: {
			if (ax_bit_has(ev.modifier, UIEventModifierKey::Atl)) {
				if (ev.pressedButtons == UIMouseEventButton::Left) {
					cam.orbit(ev.deltaPos.yx() * Vec2f(1,-1) * 0.005f);
					
				} else if (ev.pressedButtons == UIMouseEventButton::Middle) {
					cam.move(ev.deltaPos * 0.01f * _mouseSpeed);
					
				} else if (ev.pressedButtons == UIMouseEventButton::Right) {
					// cam.dolly(ev.deltaPos.y * 0.015f * _mouseSpeed);
					cam.move(Vec3f(0,0, ev.deltaPos.y * -0.02f * _mouseSpeed));
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
	if (ev.type == UIKeyEventType::Down) {
		using Op = ImUIGizmoOperation;
		switch (ev.key) {
			case UIKeyCode::Q: _opMode = Op::None;      break;
			case UIKeyCode::W: _opMode = Op::Translate; break;
			case UIKeyCode::E: _opMode = Op::Rotate;    break;
			case UIKeyCode::R: {
				_opMode = _opMode == Op::Scale ? Op::Bounds : Op::Scale;
			} break;
			case UIKeyCode::T: _opMode = Op::Universal; break;
		}
	}
}

void EditorMainWindow::MyRenderGraph::onBackBufferPass(RenderRequest* req, Span<Input> inputs) {
	Base::onBackBufferPass(req, inputs);
	_owner->_cameraDebugPanel(req);
	_owner->_statisticsPanel(req);
	_owner->_drawGizmo(req);
	_owner->_sceneOutlinerUIPanel.render(Engine::s_instance()->world(), req);
	_owner->_inspectorUIPanel.render(req);
}

void EditorMainWindow::_statisticsPanel(RenderRequest* req) {
	ImUIPanel	panel("statistics");
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

void EditorMainWindow::_cameraDebugPanel(RenderRequest* req) {
	ProjectionDesc projDesc = _renderGraph->projectionDesc();
	
	auto& cam = _renderGraph->viewportCamera();
	ImUIPanel	panel("camera");
	ImUIDragFloat("fieldOfView"   , &cam.fieldOfView, 0.1f, 5, 180);
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
	auto& cam = req->cameraData();
	auto viewMatrix = cam.viewMatrix;
	auto projMatrix = cam.projMatrix;
	float ViewManipulateOffset = 20;
	float ViewManipulateSize = 120;
	
	ImUIGizmoViewManipulate(viewMatrix,
	                        Rect2f(req->viewport().w - ViewManipulateSize - ViewManipulateOffset,
	                               ViewManipulateOffset,
	                               ViewManipulateSize,
	                               ViewManipulateSize));
	
	{
		ImUIPanel	panel("Gizmo");
		ImUIDragFloat("mouseSpeed", &_mouseSpeed, 0.1f, 0.1f, 50.0f);
		
		{
			if (ImUIRadioButton("Local", _opSpace == ImUIGizmoSpace::Local)) {
				_opSpace = ImUIGizmoSpace::Local;
			}
			
			ImUISameLine();
			if (ImUIRadioButton("World", _opSpace == ImUIGizmoSpace::World)) {
				_opSpace = ImUIGizmoSpace::World;
			}
		}
		
		ImUIDragFloat("maxMeshletErrorInPixels", &_maxMeshletErrorInPixels, 0.1f, 0, 20);
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
		
		ImUIDragFloat("showAllLodDistance", &_gpuDebugData.showAllLodDistance, 0.1f, 0, 10);
		ImUIDragFloat("Normal Length",      &_gpuDebugData.drawNormalLength, 0.01f, 0, 4);
	}
	
	req->setDebugData(_gpuDebugData);
	
	auto obj = ObjectManager::s_instance()->selection.lastSelectedObject();
	if (!obj) return;
	auto* entity = rttiCast<SceneEntity>(obj.ptr());
	if (!entity) return;

	Mat4f deltaMatrix = Mat4f::s_identity();
	Mat4f worldMatrix = entity->worldMatrix();

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
	switch (_opMode) {
		case ImUIGizmoOperation::Translate: if (_enableTranslateSnap) { snap = &_translateSnap; } break;
		case ImUIGizmoOperation::Rotate   : if (_enableRotateSnap   ) { snap = &_rotateSnap;    } break;
		case ImUIGizmoOperation::Scale    : if (_enableScaleSnap    ) { snap = &_scaleSnap;     } break;
		default: break;
	}

	BBox3f bounds = BBox3f::s_empty();
	if (auto* meshRenderer = entity->getComponent<MeshRendererComponent>()) {
		if (auto* mesh = meshRenderer->mesh.ptr()) {
			bounds = mesh->bounds();
		}
	}
	
	
	if (ImUIGizmoManipulate(viewMatrix, projMatrix, _opMode, _opSpace, snap, bounds, worldMatrix)) {
	#if 1
		entity->setWorldMatrix(worldMatrix);
	#else
		auto* parent = entity->parent();
		auto wm = Mat4d::s_cast(worldMatrix);
		
		auto localMatrix = parent ? Mat4d::s_cast(parent->worldMatrix()).inverse() * wm : wm;
		
		Vec3d  position;
		Quat4d rotation;
		Vec3d  scale;
		localMatrix.getTRS(position, rotation, scale);
		
		rotation = rotation.normalize();
		
		switch (_opMode) {
			case ImUIGizmoOperation::Translate: entity->position = Vec3f::s_cast(position);  break;
			case ImUIGizmoOperation::Rotate   : entity->rotation = Quat4f::s_cast(rotation); break;
			case ImUIGizmoOperation::Bounds: AX_FALLTHROUGH
			case ImUIGizmoOperation::Scale: {
				entity->scale    = Vec3f::s_cast(scale);
			} break;
			default: break;
		}		
#endif	
	}
}

} //namespace
