module;

module AxEditor;
import :InspectorUIPanel;

namespace AxEditor {

void InspectorUIPanel::render(RenderRequest* req) {
	ImUIPanel	outliner("Inspector");
	
	auto obj = ObjectManager::s_instance()->selection.lastSelectedObject();
	if (!obj) {
		ImUILabelText("object", "-- no selection --");
		return;
	}
	
	ImUILabelText("name", obj->name());

	if (auto* entity = rttiCast<SceneEntity>(obj.ptr())) {
		auto pos   = entity->position();
		auto rot   = entity->rotation();
		auto scale = entity->scale();
		ImUIInputFloat3  ("Position", pos);
		ImUIInputEulerDeg("Rotation", rot);
		ImUIInputQuat4   ("Quat"    , rot);
		ImUIInputFloat3  ("Scale"   , scale);

		if (auto* meshRenderer = entity->getComponent<MeshRendererComponent>()) {
			if (auto* mesh = meshRenderer->mesh.ptr()) {
				auto bounds = mesh->bounds();
				ImUIInputFloat3("Bounds min", bounds.min);
				ImUIInputFloat3("Bounds max", bounds.max);
			}
		}
		
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