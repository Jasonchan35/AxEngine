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
		ImUIInputFloat3("Position", entity->position);
		ImUIInputFloat3("Rotation", entity->rotation);
		ImUIInputFloat4("Quat"    , entity->rotation);
		ImUIInputFloat3("Scale"   , entity->scale);

		if (auto* meshRenderer = entity->getComponent<MeshRendererComponent>()) {
			if (auto* mesh = meshRenderer->renderer.mesh.ptr()) {
				ImUIInputFloat3("Bounds min", mesh->bounds().min);
				ImUIInputFloat3("Bounds max", mesh->bounds().max);
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