module;

module AxEditor;
import :InspectorUIPanel;

namespace AxEditor {

void InspectorUIPanel::render(RenderRequest* req) {
	ImUIPanel	outliner("Inspector");
	
	Array<SPtr<Object>, 64> objects;
	ObjectManager::s_instance()->selection.getSelection(objects);

	if (objects.size() <= 0) {
		ImUILabelText("object", "-- no selection --");
		return;
	}
	
	auto* obj = objects.back().ptr();

	ImUILabelText("name", obj->name());

	
	if (auto* entity = rttiCast<SceneEntity>(obj)) {
		ImUILabelText("position", Fmt("{}", entity->transform.position));
		ImUILabelText("rotation", Fmt("{}", entity->transform.rotation.eulerDeg()));
		ImUILabelText("scale"   , Fmt("{}", entity->transform.scale   ));
		
		Int componentCount = entity->componentCount();
		{
			ImUITreeNodeFlags flags;
			flags.hasChild = componentCount > 0;
			flags.open = true;
			ImUITreeNode node("Components", flags);
		}
		
		for (Int i = 0; i < componentCount; ++i) {
			auto* comp = entity->componentAt(i);
			
			ImUITreeNodeFlags flags;
			ImUITreeNode node(Fmt("{}", comp->rtti()->name), flags);
		}
		return;
	}
}

} // namespace