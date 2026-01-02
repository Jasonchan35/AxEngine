module AxRender;

namespace ax /*::AxRender*/ {

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

RenderMesh::SubMesh& RenderMesh::getSubMeshCanAddVertices(
	PrimType primType,
	VertexLayout vertexLayout,
	VertexIndexType indexType,
	Int numVertices
) {
	for (auto& sm : _subMeshes) {
		if (sm.isEmpty()) {
			sm.create(primType, vertexLayout, indexType);
			return sm;
		}

		if (sm.vertexLayout()	!= vertexLayout) continue;
		if (sm.indexType()		!= indexType ) continue;
		if (sm.primitiveType()	!= primType  ) continue;
		if (sm.canAddVertices(numVertices))
			return sm;
	}

	return addSubMesh(primType, vertexLayout, indexType);
}

} // namespace
