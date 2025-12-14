module;
#include <imgui.h>
module AxRender;
import :UI;
import :AxImGui;

namespace ax /*::AxRender*/::UI {

bool DragFloat(StrView label, float* v, float v_speed, float v_min, float v_max) {
	return ::ImGui::DragFloat(TempString(label).c_str(), v, v_speed, v_min, v_max, showMixedValue ? mixedValueFormat : floatFormat);
}

float InputFloat(StrView label, float* v) {
	return ::ImGui::InputFloat(TempString(label).c_str(), v, 0, 0, showMixedValue ? mixedValueFormat : floatFormat, ImGuiInputTextFlags_EnterReturnsTrue);
}

Window::Window(StrView name, bool* pOpen) {
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
	::ImGui::Begin(TempString(name).c_str(), pOpen, flags);
}

Window::~Window() { ImGui::End(); }

TreeNode::TreeNode(StrView label) {
	ImGuiTreeNodeFlags flags = 0;
	_isOpen = ::ImGui::TreeNodeEx(TempString(label).c_str(), flags); 
}

TreeNode::~TreeNode() {
	if (_isOpen) ImGui::TreePop();
}

CollapsingHeader::CollapsingHeader(StrView label) { ImGui::CollapsingHeader(TempString(label).c_str(), &_visiable); }

bool IsItemClicked()	{ return ::ImGui::IsItemClicked(); }
bool IsKeyShift()		{ return ::ImGui::GetIO().KeyShift; }
bool IsKeyAlt()			{ return ::ImGui::GetIO().KeyAlt; }
bool IsKeyCtrl()		{ return ::ImGui::GetIO().KeyCtrl; }

PushID::~PushID() { ImGui::PopID(); }
PushID::PushID(const void* id) { ImGui::PushID(id); }
PushID::PushID(int id) { ImGui::PushID(id); }

} // namespace