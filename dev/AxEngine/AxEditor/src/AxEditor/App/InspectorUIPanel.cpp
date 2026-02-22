module;

module AxEditor;
import :InspectorUIPanel;

namespace AxEditor {

void InspectorUIPanel::render(RenderRequest* req) {
	ImUI_InspectorRequest inspectorReq;
	inspectorReq.renderRequest = req;
	
	ImUI_Panel	outliner("Inspector");
	
	auto obj = ObjectManager::s_instance()->selection.lastSelectedObject();
	if (!obj) {
		ImUI_LabelText("object", "-- no selection --");
		return;
	}
	
	ImUI_LabelText("name", obj->name().toString());

	if (auto* entity = rttiCast<SceneEntity>(obj.ptr())) {
		_renderEntity(&inspectorReq, entity);
	}
}

void InspectorUIPanel::_renderEntity(ImUI_InspectorRequest* req, SceneEntity* entity) {
	auto pos   = entity->position();
	auto rot   = entity->rotation();
	auto scale = entity->scale();
	if (ImUI_DragFloat3 ("Position", pos)) { entity->setPosition(pos); }
	if (ImUI_DragEuler  ("Rotation", rot)) { entity->setRotation(rot); }
	if (ImUI_DragFloat3 ("Scale"   , scale)) { entity->setScale(scale); }
		
	ImUI_InputQuat4 ("Quat"    , rot);

	ImUI_Text(Fmt("WorldPos: {}", entity->worldPosition()));

	if (auto* meshRenderer = entity->getComponent<MeshRendererComponent>()) {
		if (auto* mesh = meshRenderer->mesh.ptr()) {
			auto bounds = mesh->bounds();
			ImUI_InputFloat3("Bounds min", bounds.min);
			ImUI_InputFloat3("Bounds max", bounds.max);
		}
	}
	
	for (auto& comp : entity->components()) {
		if (!comp) continue;
		_renderComponent(req, comp);
	}
}

void InspectorUIPanel::_renderComponent(ImUI_InspectorRequest* req, SceneComponent* comp) {
	ImUI_TreeNodeFlags flags;
	flags.hasChild = true;
	flags.open = true;
	ImUI_TreeNode node(Fmt("{} ({})", comp->name(), comp->rtti()->name), flags);
	if (!node.isOpen()) return;
	
	comp->onInspector(req);
}

} // namespace