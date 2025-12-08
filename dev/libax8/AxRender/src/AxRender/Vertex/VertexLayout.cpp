module AxRender;
import :Vertex;

namespace ax::AxRender {

VertexLayoutManager::VertexLayoutManager() {
}

VertexLayoutManager* VertexLayoutManager::s_instance() {
	static GlobalSingleton<VertexLayoutManager> s;
	return s.ptr();
}

VertexLayout VertexLayoutManager::registerLayout(const VertexLayoutDesc& desc) {
	auto md = _md.scopedLock();

	auto* node = md->table.findNode(desc);
	if (node) return node->value();

	auto& newNode = md->table.addNode(desc);
	newNode.value() = VertexLayout(&newNode.key()); // use Desc pointer from key

	return newNode.value();
}

}