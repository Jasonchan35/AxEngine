module;

export module AxRender:RenderMeshEdit;
export import :RenderMesh;
export import :EditableMesh;
export import :Font;

export namespace ax {

template<class COLOR>
struct ColorPair {
	COLOR c0;
	COLOR c1;;
};

class RenderMeshEdit : public NonCopyable {
public:
	using Color4fPair		= ColorPair<Color4f>;

	using AttrId			= VertexSemantic;
	using PrimType          = RenderPrimitiveType;

	void createRect				(VertexLayout vertexLayout, const Rect2f& rect, const Rect2f& uv, const Color4f& color);
	void createBorderRect		(VertexLayout vertexLayout, const Rect2f& rect, const Margin2f& border, const Rect2f& uv, const Margin2f& uvBorder, const Color4f& color, bool hasCenter);

	void addRect				(VertexLayout vertexLayout, const Rect2f& rect, const Rect2f& uv, const Color4f& color);
	void addBorderRect			(VertexLayout vertexLayout, const Rect2f& rect, const Margin2f& border, const Rect2f& uv, const Margin2f& uvBorder, const Color4f& color, bool hasCenter);

	void createCube				(VertexLayout vertexLayout, const Vec3f& pos, const Vec3f& size, const Color4f& color);

	void createText				(VertexLayout vertexLayout, StrView text, const Vec2f& pos, const FontStyle* style);
	void addText				(VertexLayout vertexLayout, StrView text, const Vec2f& pos, const FontStyle* style);

	void createTextBillboard	(VertexLayout vertexLayout, StrView text, const Vec3f& pos, const FontStyle* style);
	void addTextBillboard		(VertexLayout vertexLayout, StrView text, const Vec3f& pos, const FontStyle* style);

	void createGrid				(VertexLayout vertexLayout,
								 float cellSize, Int cellCount,
								 const Color4f& centerLineColor  = Color4f(1.0,  1.0f, 1.0f),
								 const Color4f& gridLineColor    = Color4f(0.4f, 0.4f, 0.4f),
								 const Color4f& gridLine2_Color  = Color4f(.65f, .65f, .65f),
								 Int     gridLine2_Interval = 5); 

	void createLines			(VertexLayout vertexLayout, Span<Vec3f> positions, const Color4f& color);
	void createLines			(VertexLayout vertexLayout, Span<Vec2f> positions, const Color4f& color);

	void createLineStrip		(VertexLayout vertexLayout, Span<Vec3f> positions, const Color4f& color);
	void createLineStrip		(VertexLayout vertexLayout, Span<Vec2f> positions, const Color4f& color);

	template<class SRC>
	void createLinesFromVertexNormals(VertexLayout vertexLayout, const SRC& src, float normalLength, const Color4fPair& color) {
		_mesh.clear();
		addLinesFromVertexNormals(vertexLayout, src, normalLength, color);
	}

	void addLinesFromVertexNormals			(VertexLayout vertexLayout, const RenderMesh&    srcMesh,      float normalLength, const Color4fPair& color);
	void addLinesFromVertexNormals			(VertexLayout vertexLayout, const RenderSubMesh& srcSubMesh,   float normalLength, const Color4fPair& color);
	void addLinesFromVertexNormals			(VertexLayout vertexLayout, const VertexBuffer&  vertexBuffer, float normalLength, const Color4fPair& color);

	void createFromEditableMesh				(VertexLayout vertexLayout, EditableMesh& srcMesh);
	void addFromEditableMesh				(VertexLayout vertexLayout, EditableMesh& srcMesh);

	void createLinesFromEdges				(VertexLayout vertexLayout, EditableMesh& srcMesh, const Color4f& color);
	void createLinesFromFaceEdges			(VertexLayout vertexLayout, EditableMesh& srcMesh, const Color4f& color);

	void createLinesFromVertexNormals		(VertexLayout vertexLayout, EditableMesh& srcMesh, float normalLength, const Color4fPair& color);
	void createLinesFromEdgeNormals			(VertexLayout vertexLayout, EditableMesh& srcMesh, float normalLength, const Color4fPair& color);
	void createLinesFromFaceNormals			(VertexLayout vertexLayout, EditableMesh& srcMesh, float normalLength, const Color4fPair& color);
	void createLinesFromFaceVertexNormals	(VertexLayout vertexLayout, EditableMesh& srcMesh, float normalLength, const Color4fPair& color);

	void createSphere						(VertexLayout vertexLayout, float radius, Int rows, Int columns);
	void createCylinder						(VertexLayout vertexLayout, float height, float radius, Int rows, Int columns, bool topCap, bool bottomCap);
	void createCone							(VertexLayout vertexLayout, float height, float radius, Int rows, Int columns, bool bottomCap);

	void createTextFromPointIds				(VertexLayout vertexLayout, EditableMesh& srcMesh, const FontStyle* fontStyle);
	void createTextFromFaceIds				(VertexLayout vertexLayout, EditableMesh& srcMesh, const FontStyle* fontStyle);
	void createTextFromEdgeIds				(VertexLayout vertexLayout, EditableMesh& srcMesh, const FontStyle* fontStyle);
	void createTextFromFaceEdgeIds			(VertexLayout vertexLayout, EditableMesh& srcMesh, const FontStyle* fontStyle);

	void setColor(const Color4f color, Int colorSet = 0);

friend class RenderMesh;
protected:
	RenderMeshEdit(RenderMesh& mesh) : _mesh(mesh) {}

private:
	RenderMesh&	_mesh;
};

bool RenderSubMesh::canAddVertices(Int n) const {
	return vertexCount() + n <= indexBuffer.indexLimit();
}

RenderMesh::SubMesh& RenderMesh::addSubMesh(PrimType primType, VertexLayout vertexLayout, VertexIndexType indexType) {
	setSubMeshCount(_subMeshes.size() + 1);
	auto& sm = _subMeshes.back();
	sm.create(primType, vertexLayout, indexType);
	return sm;	
}

inline  RenderMeshEdit RenderMesh::edit() { return RenderMeshEdit(*this); }

}// namespace 
