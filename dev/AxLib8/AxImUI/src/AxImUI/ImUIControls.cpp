module;

module AxImUI;
import :ImUIControls;

namespace ax::ImUI {
	bool IsItemClicked()	{ return ::ImGui::IsItemClicked(); }
	bool IsKeyShift()		{ return ::ImGui::GetIO().KeyShift; }
	bool IsKeyAlt()			{ return ::ImGui::GetIO().KeyAlt; }
	bool IsKeyCtrl()		{ return ::ImGui::GetIO().KeyCtrl; }
}

namespace ax {

void ImUISeparator() {
	ImGui::Separator();
}

void ImUISameLine() {
	ImGui::SameLine();
}

void ImUINewLine() {
	ImGui::NewLine();
}

bool ImUIGizmoManipulate(const Mat4f&       viewMatrix,
                         const Mat4f&       projMatrix,
                         ImUIGizmoOperation op,
                         ImUIGizmoSpace     space,
                         const Vec3f*       snap,
                         const BBox3f&      bounds,
                         Mat4f&             objMatrix,
                         Mat4f*             outDeltaMatrix)
{
	const float* pSnap = snap ? snap->e : nullptr;
	bool opIsBounds = op == ImUIGizmoOperation::Bounds;
	
	float localBounds[6];
	localBounds[0] = bounds.min.x;
	localBounds[1] = bounds.min.y;
	localBounds[2] = bounds.min.z;
	localBounds[3] = bounds.max.x;
	localBounds[4] = bounds.max.y;
	localBounds[5] = bounds.max.z;
	
	bool hasLocalBounds = opIsBounds && bounds.isValid();
	
	bool ret = ImGuizmo::Manipulate(viewMatrix.e,
									projMatrix.e,
									static_cast<ImGuizmo::OPERATION>(op),
									static_cast<ImGuizmo::MODE>(space),
									objMatrix.e,
									outDeltaMatrix ? outDeltaMatrix->e : nullptr,
									opIsBounds ? nullptr : pSnap, 
									hasLocalBounds ? localBounds : nullptr, 
									opIsBounds ? pSnap : nullptr);
	return ret;
}

void ImUIGizmoViewManipulate(Mat4f& viewMatrix, const Rect2f& rect) {
	float distance = 50;
	ImVec2 pos(rect.x, rect.y);
	ImVec2 size(rect.w, rect.h);
	ImU32 bgColor = 0x00000000;
	ImGuizmo::ViewManipulate(viewMatrix.e, distance, pos, size, bgColor);
}

ImDrawList* ImUIGetDrawList() {
	return ImGuizmo::_AxImUIGetDrawList();
}

void ImUIGizmoCamera(const Rect2f&         viewport,
                     const Mat4f&          viewMatrix,
                     const Mat4f&          projMatrix,
                     Math::Camera3f&       camera,
                     const Mat4f&          cameraWorldMatrix,
                     const ProjectionDesc& projDesc) 
{
	auto matMVP = viewMatrix * projMatrix * cameraWorldMatrix;
	ImDrawList* drawList = ImUIGetDrawList();
	
	camera.aim.set(0,0,10);
	camera.setEye({0,0,0});
	camera.nearClip = 0.1f;
	camera.farClip  = 10.0f;
	
	FixedArray<ImVec2, 8> screenPoints;
	
	auto worldPoints = camera.getFrustumPoints(projDesc);
	
//	worldPoints[0] = Vec3f(0,0,0);
//	worldPoints[1] = Vec3f(5,5,5);
	
	for (Int i = 0; i < 8; i++) {
		auto clipSpace = matMVP.mulPoint(Vec4f(worldPoints[i], 1)).xyz_div_w();
		auto pt = (clipSpace.xy() * Vec2f(0.5f, -0.5f) + 0.5f) * viewport.size - viewport.pos;
		screenPoints[i] = ImVec2_make(pt);
	}

	ImU32 color = 0xffffffff;
	constexpr float thickness = -1;

	drawList->AddLine(screenPoints[0], screenPoints[1], color, thickness);
	drawList->AddLine(screenPoints[1], screenPoints[2], color, thickness);
	drawList->AddLine(screenPoints[2], screenPoints[3], color, thickness);
	drawList->AddLine(screenPoints[3], screenPoints[0], color, thickness);

	drawList->AddLine(screenPoints[4], screenPoints[5], color, thickness);
	drawList->AddLine(screenPoints[5], screenPoints[6], color, thickness);
	drawList->AddLine(screenPoints[6], screenPoints[7], color, thickness);
	drawList->AddLine(screenPoints[7], screenPoints[4], color, thickness);
	
	drawList->AddLine(screenPoints[0], screenPoints[4], color, thickness);
	drawList->AddLine(screenPoints[1], screenPoints[5], color, thickness);
	drawList->AddLine(screenPoints[2], screenPoints[6], color, thickness);
	drawList->AddLine(screenPoints[3], screenPoints[7], color, thickness);
}

bool ImUIGizmoIsUsing() {
	return ImGuizmo::IsUsing();
}

bool ImUIButton(ZStrView label, Vec2f size) {
	return ::ImGui::Button(label.c_str(), ImVec2(size.x, size.y));
}

bool ImUIRadioButton(ZStrView label, bool active) {
	return ::ImGui::RadioButton(label.c_str(), active);
}

bool ImUIColorButton(ZStrView label, const Color4f& color) {
	auto c = ImVec4(color.r, color.g, color.b, color.a);
	return ::ImGui::ColorButton(label.c_str(), c, ImGuiColorEditFlags_None);
}

bool ImUICheckBox(ZStrView label, bool& v) {
	return ::ImGui::Checkbox(label.c_str(), &v);
}

ImDrawList* ImUIGetWindowDrawList() {
	return ImGui::GetWindowDrawList();
}

void ImUIText(ZStrView text) {
	::ImGui::Text("%s", text.c_str());
}

void ImUILabelText(ZStrView label, ZStrView text) {
	return ::ImGui::LabelText(label.c_str(), "%s", text.c_str());
}

bool ImUIInputFloat3(ZStrView label, Vec3f& value) {
	return ImGui::InputFloat3(label.c_str(), value.e, "%g", ImGuiInputTextFlags_None);
}

bool ImUIInputFloat3(ZStrView label, Quat4f& value) {
	Vec3f euler = value.eulerDeg();
	if (ImUIInputFloat3(label, euler)) {
		value.setEulerDeg(euler);
		return true;
	}
	return false;
}

bool ImUIInputFloat4(ZStrView label, Quat4f& value) {
	return ImGui::InputFloat4(label.c_str(), value.e, "%g", ImGuiInputTextFlags_None);
}

bool ImUIDragFloat(ZStrView label, float* v, float v_speed, float v_min, float v_max) {
	return ImGui::DragFloat(label.c_str(),
	                          v,
	                          v_speed,
	                          v_min,
	                          v_max,
	                          ImUI::showMixedValue ? ImUI::mixedValueFormat : ImUI::floatFormat, 
	                          ImGuiSliderFlags_AlwaysClamp);
}

bool ImUIDragInt(ZStrView label, i32* v, float v_speed, i32 v_min, i32 v_max) {
	return ImGui::DragInt(label.c_str(), v, v_speed, v_min, v_max, "%d", ImGuiSliderFlags_AlwaysClamp);
}

float ImUIInputFloat(ZStrView label, float* v) {
	return ImGui::InputFloat(label.c_str(),
	                           v,
	                           0,
	                           0,
	                           ImUI::showMixedValue ? ImUI::mixedValueFormat : ImUI::floatFormat,
	                           ImGuiInputTextFlags_EnterReturnsTrue);
}

ImUIPanel::ImUIPanel(ZStrView name, bool* pOpen) {
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
	::ImGui::Begin(name.c_str(), pOpen, flags);
}

ImUIPanel::~ImUIPanel() { ImGui::End(); }

ImUITreeNode::ImUITreeNode(ZStrView label, ImUITreeNodeFlags flags) {
	ImGuiTreeNodeFlags outFlags = ImGuiTreeNodeFlags_DrawLinesFull
	                              | ImGuiTreeNodeFlags_OpenOnArrow
	                              | ImGuiTreeNodeFlags_OpenOnDoubleClick
	                              | ImGuiTreeNodeFlags_SpanFullWidth;

	if (!flags.hasChild) outFlags |= ImGuiTreeNodeFlags_Leaf;
	if (flags.open     ) outFlags |= ImGuiTreeNodeFlags_DefaultOpen;
	if (flags.selected ) outFlags |= ImGuiTreeNodeFlags_Selected;
	
	_isOpen = ::ImGui::TreeNodeEx(label.c_str(), outFlags); 
}

ImUITreeNode::~ImUITreeNode() {
	if (_isOpen) ImGui::TreePop();
}

ImUICollapsingHeader::ImUICollapsingHeader(ZStrView label) { ImGui::CollapsingHeader(label.c_str(), &_visiable); }

ImUIPushID::~ImUIPushID() { ImGui::PopID(); }
ImUIPushID::ImUIPushID(const void* id) { ImGui::PushID(id); }
ImUIPushID::ImUIPushID(int id) { ImGui::PushID(id); }

} // namespace