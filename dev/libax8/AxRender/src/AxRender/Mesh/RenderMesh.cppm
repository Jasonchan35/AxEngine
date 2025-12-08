module;
export module AxRender:RenderMesh;
export import :VertexBuffer;

export namespace ax::AxRender {

class RenderMesh;

class RenderSubMesh : public NonCopyable {
public:
	void clear();

	template<class VERTEX, class INDEX>
	void create(Span<VERTEX> vertexData, Span<INDEX> indexData) {
		vertexBuffer.addVertices(vertexData);
		indexBuffer.addIndices(indexData);
	}

	AX_INLINE	void			setPrimitiveType(PrimitiveType t)	{ _primitiveType = t; }
	AX_INLINE	PrimitiveType	primitiveType() const				{ return _primitiveType; }


	AX_INLINE	VertexLayout	vertexLayout() const	{ return vertexBuffer.vertexLayout(); }
	AX_INLINE	Int				vertexCount() const		{ return vertexBuffer.vertexCount(); }

	AX_INLINE	IndexType		indexType() const		{ return indexBuffer.indexType(); }
	AX_INLINE	Int				indexCount() const		{ return indexBuffer.indexCount(); }

	VertexBuffer	vertexBuffer;
	IndexBuffer		indexBuffer;

	RenderMesh*		mesh() { return _mesh; }

friend class RenderMesh;
protected:
	PrimitiveType _primitiveType = PrimitiveType::Triangles;
	RenderMesh*	_mesh = nullptr;
};

class RenderMesh : public NonCopyable {
public:
	using SubMesh = RenderSubMesh;

	void clear();

	template<class VERTEX, class INDEX>
	void create(Span<VERTEX> vertexData, Span<INDEX> indexData);

	void create(VertexLayout vertexLayout, IndexType indexType);

	MutSpan<SubMesh>	subMeshes()				{ return _subMeshes; }
	   Span<SubMesh>	subMeshes() const		{ return _subMeshes; }

	void setSubMeshCount(Int newSize);

private:
	Array<SubMesh, 1>	_subMeshes;
};

//--------

template<class VERTEX, class INDEX> inline
void RenderMesh::create(Span<VERTEX> vertexData, Span<INDEX> indexData) {
	setSubMeshCount(1);
	_subMeshes[0].create(vertexData, indexData);
}

} // namespace