module;

module AxImUI;
import :ImUIControls;

namespace ax {

bool ImUI_IsItemClicked()	{ return ::ImGui::IsItemClicked(); }
bool ImUI_IsKeyShift()		{ return ::ImGui::GetIO().KeyShift; }
bool ImUI_IsKeyAlt()		{ return ::ImGui::GetIO().KeyAlt; }
bool ImUI_IsKeyCtrl()		{ return ::ImGui::GetIO().KeyCtrl; }

void ImUI_Separator() { ImGui::Separator(); }
void ImUI_SameLine() { ImGui::SameLine(); }
void ImUI_NewLine() { ImGui::NewLine(); }
































































































bool ImUI_Button(ZStrView label, Vec2f size) {
	return ::ImGui::Button(label.c_str(), ImVec2(size.x, size.y));
}

bool ImUI_RadioButton(ZStrView label, bool active) {
	return ::ImGui::RadioButton(label.c_str(), active);
}

bool ImUI_ColorButton(ZStrView label, const Color4f& color) {
	auto c = ImVec4(color.r, color.g, color.b, color.a);
	return ::ImGui::ColorButton(label.c_str(), c, ImGuiColorEditFlags_None);
}

bool ImUI_CheckBox(ZStrView label, bool& v) {
	return ::ImGui::Checkbox(label.c_str(), &v);
}

ImDrawList* ImUI_GetWindowDrawList() {
	return ImGui::GetWindowDrawList();
}

void ImUI_Text(ZStrView text) {
	::ImGui::Text("%s", text.c_str());
}

void ImUI_LabelText(ZStrView label, ZStrView text) {
	return ::ImGui::LabelText(label.c_str(), "%s", text.c_str());
}

bool ImUI_InputFloat2(ZStrView label, Vec2f& value) {
	return ImGui::InputFloat2(label.c_str(), value.e, "%g", ImGuiInputTextFlags_None);
}

bool ImUI_InputFloat3(ZStrView label, Vec3f& value) {
	return ImGui::InputFloat3(label.c_str(), value.e, "%g", ImGuiInputTextFlags_None);
}

bool ImUI_InputFloat4(ZStrView label, Vec4f& value) {
	return ImGui::InputFloat4(label.c_str(), value.e, "%g", ImGuiInputTextFlags_None);
}

bool ImUI_InputEulerDeg(ZStrView label, Quat4f& value) {
	Vec3f euler = value.euler();
	if (ImUI_InputFloat3(label, euler)) {
		value.setEuler(euler);
		return true;
	}
	return false;
}

bool ImUI_InputQuat4(ZStrView label, Quat4f& value) {
	return ImGui::InputFloat4(label.c_str(), value.e, "%g", ImGuiInputTextFlags_None);
}

bool ImUI_InputMat4(ZStrView label, Mat4f& value) {
	ImUI_Text(label);
	bool b = false;
	b |= ImUI_InputFloat4("", value.cx);
	b |= ImUI_InputFloat4("", value.cy);
	b |= ImUI_InputFloat4("", value.cw);
	b |= ImUI_InputFloat4("", value.cz);
	return b;
}

bool ImUI_DragFloat(ZStrView label, float& v, float v_speed, float v_min, float v_max) {
	return ImGui::DragFloat(label.c_str(),
	                          &v,
	                          v_speed,
	                          v_min,
	                          v_max,
	                          ImUI_showMixedValue ? ImUI_mixedValueFormat : ImUI_floatFormat, 
	                          ImGuiSliderFlags_AlwaysClamp);
}

bool ImUI_DragColor(ZStrView label, Color3f& value, float v_speed, float v_min, float v_max) {
	return ImGui::DragFloat3(label.c_str(), value.e, v_speed, v_min, v_max);
}

bool ImUI_DragColor(ZStrView label, Color4f& value, float v_speed, float v_min, float v_max) {
	return ImGui::DragFloat4(label.c_str(), value.e, v_speed, v_min, v_max);
}

bool ImUI_DragFloat2(ZStrView label, Vec2f& value, float v_speed, float v_min, float v_max) {
	return ImGui::DragFloat2(label.c_str(), value.e, v_speed, v_min, v_max);
}

bool ImUI_DragFloat3(ZStrView label, Vec3f& value, float v_speed, float v_min, float v_max) {
	return ImGui::DragFloat3(label.c_str(), value.e, v_speed, v_min, v_max);
}

bool ImUI_DragFloat4(ZStrView label, Vec3f& value, float v_speed, float v_min, float v_max) {
	return ImGui::DragFloat4(label.c_str(), value.e, v_speed, v_min, v_max);
}

bool ImUI_DragEuler(ZStrView label, Quat4f& value, float v_speed, float v_min, float v_max) {
	auto e = value.euler();
	if (ImUI_DragFloat3(label, e, v_speed, v_min, v_max)) {
		value.setEuler(e);
		return true;
	}
	return false;
}

bool ImUI_DragInt(ZStrView label, i32& v, float v_speed, i32 v_min, i32 v_max) {
	return ImGui::DragInt(label.c_str(), &v, v_speed, v_min, v_max, "%d", ImGuiSliderFlags_AlwaysClamp);
}

float ImUI_InputFloat(ZStrView label, float& v) {
	return ImGui::InputFloat(label.c_str(),
	                           &v, 0, 0,
	                           ImUI_showMixedValue ? ImUI_mixedValueFormat : ImUI_floatFormat,
	                           ImGuiInputTextFlags_EnterReturnsTrue);
}

ImUI_Panel::ImUI_Panel(ZStrView name, bool* pOpen) {
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
	::ImGui::Begin(name.c_str(), pOpen, flags);
}

ImUI_Panel::~ImUI_Panel() { ImGui::End(); }

ImUI_TreeNode::ImUI_TreeNode(ZStrView label, ImUI_TreeNodeFlags flags) {
	ImGuiTreeNodeFlags outFlags = ImGuiTreeNodeFlags_DrawLinesFull
	                              | ImGuiTreeNodeFlags_OpenOnArrow
	                              | ImGuiTreeNodeFlags_OpenOnDoubleClick
	                              | ImGuiTreeNodeFlags_SpanFullWidth;

	if (!flags.hasChild) outFlags |= ImGuiTreeNodeFlags_Leaf;
	if (flags.open     ) outFlags |= ImGuiTreeNodeFlags_DefaultOpen;
	if (flags.selected ) outFlags |= ImGuiTreeNodeFlags_Selected;
	
	_isOpen = ::ImGui::TreeNodeEx(label.c_str(), outFlags); 
}

ImUI_TreeNode::~ImUI_TreeNode() {
	if (_isOpen) ImGui::TreePop();
}

ImUI_CollapsingHeader::ImUI_CollapsingHeader(ZStrView label) { ImGui::CollapsingHeader(label.c_str(), &_visiable); }

ImUI_ScopedPushId::~ImUI_ScopedPushId() { ImGui::PopID(); }
ImUI_ScopedPushId::ImUI_ScopedPushId(const void* id) { ImGui::PushID(id); }
ImUI_ScopedPushId::ImUI_ScopedPushId(int id) { ImGui::PushID(id); }

} // namespace