module;
export module AxRender:RenderMesh;
export import :VertexBuffer;

export namespace ax /*::AxRender*/ {

class RenderMeshEdit;
class RenderMesh;

class RenderSubMesh : public NonCopyable {
public:
	using PrimType = RenderPrimitiveType;
	void clear();

	void create(RenderPrimitiveType primitiveType, VertexLayout vertexLayout, VertexIndexType indexType) {
		setPrimitiveType(primitiveType);
		vertexBuffer.create(vertexLayout);
		indexBuffer.create(indexType);
	}
	
	template<class VERTEX, class INDEX>
	void create(Span<VERTEX> vertexData, Span<INDEX> indexData) {
		vertexBuffer.addVertices(vertexData);
		indexBuffer.addIndices(indexData);
	}

	AX_INLINE	void		setPrimitiveType(PrimType t)	{ _primType = t; }
	AX_INLINE	PrimType	primitiveType() const			{ return _primType; }

	bool canAddVertices	(Int n) const;

	AX_INLINE	VertexLayout	vertexLayout() const	{ return vertexBuffer.vertexLayout(); }
	AX_INLINE	Int				vertexCount() const		{ return vertexBuffer.vertexCount(); }

	AX_INLINE	VertexIndexType	indexType() const		{ return indexBuffer.indexType(); }
	AX_INLINE	Int				indexCount() const		{ return indexBuffer.indexCount(); }

	bool isEmpty() { return vertexBuffer.isEmpty() && indexBuffer.isEmpty(); }

	VertexBuffer		vertexBuffer;
	VertexIndexBuffer	indexBuffer;

	RenderMesh*		mesh() { return _mesh; }

friend class RenderMesh;
protected:
	PrimType _primType = PrimType::Triangles;
	RenderMesh*	_mesh = nullptr;
};

class RenderMesh : public NonCopyable {
public:
	using SubMesh = RenderSubMesh;
	using PrimType = RenderPrimitiveType;

	void clear();

	template<class VERTEX, class INDEX>
	void create(Span<VERTEX> vertexData, Span<INDEX> indexData);
	void create(VertexLayout vertexLayout, VertexIndexType indexType);

	MutSpan<SubMesh>	subMeshes()				{ return _subMeshes; }
	   Span<SubMesh>	subMeshes() const		{ return _subMeshes; }

	SubMesh& addSubMesh	(PrimType primType, VertexLayout vertexLayout, VertexIndexType indexType);
	
	void setSubMeshCount(Int newSize);
	SubMesh& getSubMeshCanAddVertices(PrimType primType, VertexLayout vertexLayout, VertexIndexType indexType, Int numVertices);

	template<class VERTEX>
	struct EditVertexT {
		SubMesh&		subMesh;
		IntRange		range{0,0};
		MutSpan<VERTEX>	vertices;
	};	
	
	template<class VERTEX>
	EditVertexT<VERTEX> addVertices(PrimType primType, VertexIndexType indexType, Int n) {
		auto& sm = getSubMeshCanAddVertices(primType, VERTEX::s_desc(), indexType, n);
		sm.vertexBuffer.addVertices(sm.vertices);
		
		IntRange range(sm.vertices.size(), n);
		auto v = sm.vertices.template add<VERTEX>(n);
		return {sm, range, v};
	}

	template<class VERTEX, class INDEX_TYPE>
	EditVertexT<VERTEX> addVertices(PrimType primType, Int n) {
		return addVertices<VERTEX>(primType, VertexIndexType_get<INDEX_TYPE>, n);
	}

	struct EditVertex {
		SubMesh& subMesh;
		IntRange range;
		template<class ELEM> using ElemEnumerator = VertexBuffer::ElemEnumerator_<ELEM>;
		
		bool hasElement(VertexSemantic sem) const { return subMesh.vertexLayout()->find(sem) != nullptr; }
	
		template<class ELEM>
		Opt<ElemEnumerator<ELEM>> tryEditElements(VertexSemantic semantic) {
			return subMesh.vertexBuffer.tryEditElements<ELEM>(semantic, range);
		}

		template<class ELEM = f32x3> Opt<ElemEnumerator<ELEM>> tryEditPosition () { return tryEditElements<ELEM>(VertexSemantic::POSITION ); }
		template<class ELEM = f32x3> Opt<ElemEnumerator<ELEM>> tryEditNormal0  () { return tryEditElements<ELEM>(VertexSemantic::NORMAL0  ); }

		template<class ELEM = f32x4> Opt<ElemEnumerator<ELEM>> tryEditColor(Int i) { return tryEditElements<ELEM>(VertexSemantic::COLOR0 + i); }
		template<class ELEM = f32x4> Opt<ElemEnumerator<ELEM>> tryEditColor0   () { return tryEditElements<ELEM>(VertexSemantic::COLOR0   ); }

		template<class ELEM = f32x4> Opt<ElemEnumerator<ELEM>> tryEditTexCoord(Int i) { return tryEditElements<ELEM>(VertexSemantic::TEXCOORD0 + i); }
		template<class ELEM = f32x2> Opt<ElemEnumerator<ELEM>> tryEditTexCoord0() { return tryEditElements<ELEM>(VertexSemantic::TEXCOORD0); }
		template<class ELEM = f32x2> Opt<ElemEnumerator<ELEM>> tryEditTexCoord1() { return tryEditElements<ELEM>(VertexSemantic::TEXCOORD1); }
		template<class ELEM = f32x2> Opt<ElemEnumerator<ELEM>> tryEditTexCoord2() { return tryEditElements<ELEM>(VertexSemantic::TEXCOORD2); }
		
		template<class INDEX>
		void addIndices(Span<INDEX> data) { addIndices(data.toByteSpan(), VertexIndexType_get<INDEX>); }
		void addIndices(ByteSpan data, VertexIndexType indexType) { return subMesh.indexBuffer.addIndices(data, indexType); }
		
		template<class INDEX>
		MutSpan<INDEX> addIndices(Int count) {
			subMesh.indexBuffer.addIndices(count, VertexIndexType_get<INDEX>);
		}
	};
	
	EditVertex addVertices(PrimType primType, VertexLayout vertexLayout, VertexIndexType indexType, Int newVertexCount) {
		auto& sm = getSubMeshCanAddVertices(primType, vertexLayout, indexType, newVertexCount);
		auto range = IntRange_StartAndSize(sm.vertexBuffer.vertexCount(), newVertexCount);
		auto mutSpan = sm.vertexBuffer.addVertices(newVertexCount);
		return {sm, range};
	}
	
	
	RenderMeshEdit edit();
	
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