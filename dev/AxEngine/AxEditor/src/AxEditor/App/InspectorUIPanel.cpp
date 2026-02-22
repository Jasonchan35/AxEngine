module;

module AxEditor;
import :InspectorUIPanel;

namespace AxEditor {

void InspectorUIPanel::render(RenderRequest* req) {
	ImUI_Panel	outliner("Inspector");
	
	auto obj = ObjectManager::s_instance()->selection.lastSelectedObject();
	if (!obj) {
		ImUI_LabelText("object", "-- no selection --");
		return;
	}
	
	ImUI_LabelText("name", obj->name().toString());

	if (auto* entity = rttiCast<SceneEntity>(obj.ptr())) {
		_renderEntity(entity);
	}
}

void InspectorUIPanel::_renderEntity(SceneEntity* entity) {
	auto pos   = entity->position();
	auto rot   = entity->rotation();
	auto scale = entity->scale();
	if (ImUI_DragFloat3 ("Position", pos)) { entity->setPosition(pos); }
	if (ImUI_DragEuler  ("Rotation", rot)) { entity->setRotation(rot); }
	if (ImUI_DragFloat3 ("Scale"   , scale)) { entity->setScale(scale); }
		
	ImUI_InputQuat4 ("Quat"    , rot);

	if (auto* meshRenderer = entity->getComponent<MeshRendererComponent>()) {
		if (auto* mesh = meshRenderer->mesh.ptr()) {
			auto bounds = mesh->bounds();
			ImUI_InputFloat3("Bounds min", bounds.min);
			ImUI_InputFloat3("Bounds max", bounds.max);
		}
	}
		
	Int componentCount = entity->componentCount();
	{
		ImUI_TreeNodeFlags flags;
		flags.hasChild = componentCount > 0;
		flags.open = true;
		ImUI_TreeNode node("Components", flags);
		
		for (Int i = 0; i < componentCount; ++i) {
			auto* comp = entity->componentAt(i);
			_renderComponent(comp);
		}
	}	
}

void InspectorUIPanel::_renderComponent(SceneComponent* comp) {
	ImUI_TreeNodeFlags flags;
	ImUI_TreeNode node(Fmt("{}", comp->rtti()->name), flags);
}

} // namespace