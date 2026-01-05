module;

module AxImUI;
import :ImUIControls;

namespace AxUI {

void ImUILabelText(ZStrView label, ZStrView text) {
	return ::ImGui::LabelText(label.c_str(), "%s", text.c_str());
}

bool ImUIDragFloat(ZStrView label, float* v, float v_speed, float v_min, float v_max) {
	return ::ImGui::DragFloat(label.c_str(), v, v_speed, v_min, v_max, showMixedValue ? mixedValueFormat : floatFormat);
}

float ImUIInputFloat(ZStrView label, float* v) {
	return ::ImGui::InputFloat(label.c_str(), v, 0, 0, showMixedValue ? mixedValueFormat : floatFormat, ImGuiInputTextFlags_EnterReturnsTrue);
}

ImUIPanel::ImUIPanel(ZStrView name, bool* pOpen) {
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
	::ImGui::Begin(name.c_str(), pOpen, flags);
}

ImUIPanel::~ImUIPanel() { ImGui::End(); }

ImUITreeNode::ImUITreeNode(ZStrView label) {
	ImGuiTreeNodeFlags flags = 0;
	_isOpen = ::ImGui::TreeNodeEx(label.c_str(), flags); 
}

ImUITreeNode::~ImUITreeNode() {
	if (_isOpen) ImGui::TreePop();
}

ImUICollapsingHeader::ImUICollapsingHeader(ZStrView label) { ImGui::CollapsingHeader(label.c_str(), &_visiable); }

bool IsItemClicked()	{ return ::ImGui::IsItemClicked(); }
bool IsKeyShift()		{ return ::ImGui::GetIO().KeyShift; }
bool IsKeyAlt()			{ return ::ImGui::GetIO().KeyAlt; }
bool IsKeyCtrl()		{ return ::ImGui::GetIO().KeyCtrl; }

ImUIPushID::~ImUIPushID() { ImGui::PopID(); }
ImUIPushID::ImUIPushID(const void* id) { ImGui::PushID(id); }
ImUIPushID::ImUIPushID(int id) { ImGui::PushID(id); }

} // namespace