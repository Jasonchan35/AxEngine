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
                         Mat4f&             objMatrix,
                         BBox3f&            bbox,
                         Mat4f*             outDeltaMatrix)
{
	float localBounds[6];
	localBounds[0] = bbox.min.x;
	localBounds[1] = bbox.min.y;
	localBounds[2] = bbox.min.z;
	localBounds[3] = bbox.max.x;
	localBounds[4] = bbox.max.y;
	localBounds[5] = bbox.max.z;
	
	const float* pSnap = snap ? snap->e : nullptr;
	bool opIsBounds = op == ImUIGizmoOperation::Bounds;
	
	bool ret = ImGuizmo::Manipulate(viewMatrix.e,
									projMatrix.e,
									static_cast<ImGuizmo::OPERATION>(op),
									static_cast<ImGuizmo::MODE>(space),
									objMatrix.e,
									outDeltaMatrix ? outDeltaMatrix->e : nullptr,
									opIsBounds ? nullptr : pSnap, 
									opIsBounds ? localBounds : nullptr, 
									opIsBounds ? pSnap : nullptr);
	return ret;
}

void ImUIGizmoViewManipulate(Mat4f& viewMatrix, const Rect2f& rect) {
	float distance = 50;
	ImVec2 pos(rect.x, rect.y);
	ImVec2 size(rect.w, rect.h);
	ImU32 color = 0x00000000;
	ImGuizmo::ViewManipulate(viewMatrix.e, distance, pos, size, color);
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
	return ::ImGui::DragFloat(label.c_str(),
	                          v,
	                          v_speed,
	                          v_min,
	                          v_max,
	                          ImUI::showMixedValue ? ImUI::mixedValueFormat : ImUI::floatFormat);
}

float ImUIInputFloat(ZStrView label, float* v) {
	return ::ImGui::InputFloat(label.c_str(),
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