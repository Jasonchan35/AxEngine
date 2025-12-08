module AxRender;

namespace ax::AxRender {

void RenderMesh::clear() {
	_subMeshes.clear();
}

void RenderSubMesh::clear() {
	vertexBuffer.clear();
	indexBuffer.clear();
}

void RenderMesh::setSubMeshCount(Int newSize) {
	Int oldSize = _subMeshes.size();
	_subMeshes.resize(newSize);

	for (Int i = oldSize; i < newSize; i++) {
		_subMeshes[i]._mesh = this;
	}
}

} // namespace
