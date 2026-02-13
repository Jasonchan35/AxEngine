module;

module AxEditor;
import :SceneOutlinerUIPanel;

namespace AxEditor {

void SceneOutlinerUIPanel::render(SceneWorld* world, RenderRequest* req) {
	ImUIPanel	outliner("Scene Outliner");
	if (!world) return;
	_addNode(world->root());
}

void SceneOutlinerUIPanel::_addNode(SceneEntity* p) {
	if (!p) return;

	Int childCount = p->childCount();
	
	ImUITreeNodeFlags flags;
	flags.open     = p->editor.treeNodeIsOpen;
	flags.hasChild = childCount > 0;
	flags.selected = p->editor.selected;
	
	ImUITreeNode node(p->name(), flags);
	if (ImUI::IsItemClicked()) {
		ObjectManager::s_instance()->selection.select(p);
	}
	
	p->editor.treeNodeIsOpen = node.isOpen();

	if (node.isOpen()) {
		for (Int i = 0; i < childCount; ++i) {
			_addNode(p->childAt(i));
		}
	}
}

} // namespace 