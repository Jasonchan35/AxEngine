module;

module AxImUI;
import :ImUIControls;

namespace AxUI {

bool ImUIDragFloat(StrView label, float* v, float v_speed, float v_min, float v_max) {
	return ::ImGui::DragFloat(TempString(label).c_str(), v, v_speed, v_min, v_max, showMixedValue ? mixedValueFormat : floatFormat);
}

float ImuiInputFloat(StrView label, float* v) {
	return ::ImGui::InputFloat(TempString(label).c_str(), v, 0, 0, showMixedValue ? mixedValueFormat : floatFormat, ImGuiInputTextFlags_EnterReturnsTrue);
}

ImUIPanel::ImUIPanel(StrView name, bool* pOpen) {
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
	::ImGui::Begin(TempString(name).c_str(), pOpen, flags);
}

ImUIPanel::~ImUIPanel() { ImGui::End(); }

ImUITreeNode::ImUITreeNode(StrView label) {
	ImGuiTreeNodeFlags flags = 0;
	_isOpen = ::ImGui::TreeNodeEx(TempString(label).c_str(), flags); 
}

ImUITreeNode::~ImUITreeNode() {
	if (_isOpen) ImGui::TreePop();
}

ImUICollapsingHeader::ImUICollapsingHeader(StrView label) { ImGui::CollapsingHeader(TempString(label).c_str(), &_visiable); }

bool IsItemClicked()	{ return ::ImGui::IsItemClicked(); }
bool IsKeyShift()		{ return ::ImGui::GetIO().KeyShift; }
bool IsKeyAlt()			{ return ::ImGui::GetIO().KeyAlt; }
bool IsKeyCtrl()		{ return ::ImGui::GetIO().KeyCtrl; }

ImUIPushID::~ImUIPushID() { ImGui::PopID(); }
ImUIPushID::ImUIPushID(const void* id) { ImGui::PushID(id); }
ImUIPushID::ImUIPushID(int id) { ImGui::PushID(id); }

} // namespace