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

bool ImUIButton(ZStrView label, Vec2f size) {
	return ::ImGui::Button(label.c_str(), ImVec2(size.x, size.y));
}

void ImUILabelText(ZStrView label, ZStrView text) {
	return ::ImGui::LabelText(label.c_str(), "%s", text.c_str());
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