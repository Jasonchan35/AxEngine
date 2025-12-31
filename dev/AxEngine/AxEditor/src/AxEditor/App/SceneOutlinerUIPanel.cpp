module;

module AxEditor;
import :SceneOutlinerUIPanel;

namespace AxEditor {

void SceneOutlinerUIPanel::render(RenderRequest* req) {
	ImUIPanel	outliner("Outliner");
	static float f = 10;
	ImUIDragFloat("float", &f);
}

} // namespace 