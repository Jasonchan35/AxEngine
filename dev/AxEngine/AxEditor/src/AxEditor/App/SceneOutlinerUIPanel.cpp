module;

module AxEditor;
import :SceneOutlinerUIPanel;

namespace AxEditor {

void SceneOutlinerUIPanel::render(RenderRequest* req) {
	ImUIPanel	outliner("Scene Outliner");
	
	auto* world = SceneWorld::s_instance();
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
	if (IsItemClicked()) {
		p->editor.selected = !p->editor.selected; 
	}
	p->editor.treeNodeIsOpen = node.isOpen();
	
	if (node.isOpen()) {
		for (Int i = 0; i < childCount; ++i) {
			_addNode(p->childAt(i));
		}
	}
}

} // namespace 