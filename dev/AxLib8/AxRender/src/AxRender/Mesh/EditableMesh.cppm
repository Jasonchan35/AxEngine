module;

export module AxRender:EditableMesh;
export import :Common;

export namespace ax {


// https://en.wikipedia.org/wiki/Doubly_connected_edge_list
// The doubly connected edge list (DCEL), also known as half-edge data structure

enum class EditableMesh_PointId			: Int { Invalid = -1 };
enum class EditableMesh_EdgeId			: Int { Invalid = -1 };
enum class EditableMesh_FaceId			: Int { Invalid = -1 };
enum class EditableMesh_FaceVertexId	: Int { Invalid = -1 };
enum class EditableMesh_FaceEdgeId		: Int { Invalid = -1 };

AX_ENUM_ALL_OPERATOR(EditableMesh_PointId);
AX_ENUM_ALL_OPERATOR(EditableMesh_EdgeId);
AX_ENUM_ALL_OPERATOR(EditableMesh_FaceId);
AX_ENUM_ALL_OPERATOR(EditableMesh_FaceVertexId);
AX_ENUM_ALL_OPERATOR(EditableMesh_FaceEdgeId);

#define AX_EditableMesh_ElementType_EnumList(E) \
	E(Unknown,)    \
	E(Point,)      \
	E(Edge,)       \
	E(Face,)       \
	E(FaceVertex,) \
	E(FaceEdge,)   \
//---------
AX_ENUM_CLASS(AX_EditableMesh_ElementType_EnumList, EditableMesh_ElementType, u8)

class EditableMeshEdit;

class EditableMesh : public NonCopyable {
public:
	using Mesh		= EditableMesh;
	using PointId		= EditableMesh_PointId;
	using EdgeId		= EditableMesh_EdgeId;
	using FaceId		= EditableMesh_FaceId;
	using FaceEdgeId	= EditableMesh_FaceEdgeId;
	using ElementType	= EditableMesh_ElementType;
	using Color			= Color4f;
	using Vec2			= Vec2d;
	using Vec3			= Vec3d;
	using Vec4			= Vec4d;
	using Mat4			= Mat4d;

	struct Point;
	struct Edge;
	struct FaceEdge;

	EditableMeshEdit edit();

	//-----------------------------------------------------------------
	struct Point : public NonCopyable {
		Vec3		pos;
		Vec3		normal;
		PointId	id() const { return _id; }

	friend class EditableMesh;
	protected:
		Int		_faceCount = 0;
		PointId		_id = PointId::Invalid;
		EdgeId		_edgeHead = EdgeId::Invalid;
		EdgeId		_edgeTail = EdgeId::Invalid;
	};

	//-----------------------------------------------------------------
	struct Edge : public NonCopyable {
		Vec3		normal;
		bool		softEdge : 1;

		Edge() : softEdge(false) {}
		EdgeId	id() const { return _id; }

		Point&	point0(Mesh& mesh);
		Point&	point1(Mesh& mesh);

		bool hasPoint	(PointId p0, PointId p1);
		void addFaceEdge(Mesh& mesh, FaceEdge& fe);
		Vec3 center		(Mesh& mesh);

		void addToPoint	(Mesh& mesh, Point& v0, Point& v1);
		template<class CH>
		void onFormat	(Format_<CH>& f) const;

		EdgeId& next(PointId v); //!< next base on pointId

	friend class EditableMesh;
	protected:
		EdgeId			_id		= EdgeId::Invalid;
		PointId			_point0	= PointId::Invalid;
		PointId			_point1	= PointId::Invalid;

		FaceEdgeId		_faceEdgeHead = FaceEdgeId::Invalid;  //faceEdge shared this edge
		FaceEdgeId		_faceEdgeTail = FaceEdgeId::Invalid;

	private:
		EdgeId			_next0		= EdgeId::Invalid;  //next edge for vertex 0
		EdgeId			_next1		= EdgeId::Invalid;  //next edge for vertex 1
	};

	//-----------------------------------------------------------------
	struct FaceEdge : public NonCopyable {
		Point&		point	(Mesh& mesh);
		Vec3&		normal	(Mesh& mesh);
		Edge&		edge	(Mesh& mesh);
		FaceEdgeId	id		() const { return _id; }

	friend class EditableMesh;
	protected:
		FaceEdgeId		_id			= FaceEdgeId::Invalid;
		PointId			_point		= PointId::Invalid;
		EdgeId			_edge		= EdgeId::Invalid;
		FaceId			_face		= FaceId::Invalid;
		FaceEdgeId		_next		= FaceEdgeId::Invalid;
		FaceEdgeId		_adjacent	= FaceEdgeId::Invalid; //FaceEdge shared by the same edge
	};

	//-----------------------------------------------------------------
	struct Face : public NonCopyable {
		Vec3		normal;
		Vec3		center;
		Int		pointCount()	{ return _faceEdgeCount; }
		FaceId		id() const		{ return _id; }

		void				getPoints	(Mesh& mesh, IArray<Point*>& result);
		MutSpan<Vec3>		getNormals	(Mesh& mesh);
		MutSpan<Color>		getColors	(Mesh& mesh, Int colorSetId);
		MutSpan<Vec2>		getUvs		(Mesh& mesh, Int uvSetId);
		MutSpan<Vec4>		getCustoms	(Mesh& mesh, Int customSetId);
		MutSpan<FaceEdge>	getFaceEdges(Mesh& mesh);

	friend class EditableMesh;
	protected:
		FaceId			_id   = FaceId::Invalid;
		FaceEdgeId		_faceEdgeHead = FaceEdgeId::Invalid;
		Int			_faceEdgeCount;
	};

	//-----------------------------------------------------------------
	template<class T>
	struct FaceVertexSetBase : public NonCopyable {
		T			defaultValue;
		NameId		name;
		Array<T>	values;
	};
	struct UvSet : public FaceVertexSetBase<Vec2> {
	};
	struct ColorSet : public FaceVertexSetBase<Color> {
	};
	struct CustomSet : public FaceVertexSetBase<Vec4> {
	};

	//-----------------------------------------------------------------
	struct DisplayOptions {
		bool		normals			= false;
		bool		componentIds	= false;
		bool		doubleSided		= false;
		ElementType	elementType		= ElementType::Unknown;
	};

	//-----------------------------------------------------------------
	void	clear();

	Face&		addFace(IntSpan pointIndices);
	Point&		addPoint(const Vec3& pos);
	Point&		addPoint(double x, double y, double z) { return addPoint(Vec3(x,y,z)); }

	EdgeId		findEdge(PointId a, PointId b);

			Point&		point		(PointId    i)			{ return _points[ax_enum_int(i)]; }
	const	Point&		point		(PointId    i) const	{ return _points[ax_enum_int(i)]; }

			Edge&		edge		(EdgeId     i)			{ return _edges[ax_enum_int(i)];    }
	const	Edge&		edge		(EdgeId     i) const	{ return _edges[ax_enum_int(i)];    }

			Face&		face		(FaceId     i)			{ return _faces[ax_enum_int(i)]; }
	const	Face&		face		(FaceId     i) const	{ return _faces[ax_enum_int(i)]; }

			FaceEdge&	faceEdge	(FaceEdgeId i)			{ return _faceEdges[ax_enum_int(i)]; }
	const	FaceEdge&	faceEdge	(FaceEdgeId i) const	{ return _faceEdges[ax_enum_int(i)]; }

			Vec3&		fvNormal	(FaceEdgeId i)			{ return _fvNormals[ax_enum_int(i)]; }
	const	Vec3&		fvNormal	(FaceEdgeId i) const	{ return _fvNormals[ax_enum_int(i)]; }

	MutSpan<Point>	points()			{ return _points; }
	  Span<Point>	points() const		{ return _points; }

	MutSpan<Edge>		edges()				{ return _edges; }
	   Span<Edge>		edges() const		{ return _edges; }

	MutSpan<Face>		faces()				{ return _faces; }
	   Span<Face>		faces() const		{ return _faces; }

	MutSpan<FaceEdge>	faceEdges()			{ return _faceEdges; }
	   Span<FaceEdge>	faceEdges() const	{ return _faceEdges; }

	MutSpan<Vec3>		fvNormals()			{ return _fvNormals; }
	   Span<Vec3>		fvNormals() const	{ return _fvNormals; }

	void updateFaceNormals();
	void updateFaceVertexNormals(double hardEdgeAngleDeg);

	Int colorSetCount() const { return _fvColorSets.size(); }
	void addColorSet(const Color & defaultValue);

	Int uvSetCount() const { return _fvUvSets.size(); }
	void addUvSet(const Vec2& defaultValue);

	Int customSetCount() const { return _fvCustomSets.size(); }
	void addCustomSet(const Vec4& defaultValue);

	EditableMesh();

private:
	Array<Face>      _faces;
	Array<Point>     _points;
	Array<Edge>      _edges;
	Array<FaceEdge>  _faceEdges;
	Array<Vec3>      _fvNormals;
	Array<ColorSet>  _fvColorSets;
	Array<UvSet>     _fvUvSets;
	Array<CustomSet> _fvCustomSets;
	DisplayOptions   _displayOptions;
};

AX_INLINE
MutSpan<EditableMesh::Vec3> EditableMesh::Face::getNormals(Mesh& mesh) {
	return mesh._fvNormals.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
MutSpan<EditableMesh::Color> EditableMesh::Face::getColors(Mesh& mesh, Int colorSetId) {
	return mesh._fvColorSets[colorSetId].values.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
MutSpan<EditableMesh::Vec2> EditableMesh::Face::getUvs(Mesh& mesh, Int uvSetId) {
	return mesh._fvUvSets[uvSetId].values.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
MutSpan<EditableMesh::Vec4> EditableMesh::Face::getCustoms(Mesh& mesh, Int customSetId) {
	return mesh._fvCustomSets[customSetId].values.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
MutSpan<EditableMesh::FaceEdge> EditableMesh::Face::getFaceEdges(Mesh& mesh) {
	return mesh._faceEdges.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
EditableMesh::Point& EditableMesh::Edge::point0(Mesh& mesh) {
	return mesh.point(_point0);
}

AX_INLINE
EditableMesh::Point& EditableMesh::Edge::point1(Mesh& mesh) {
	return mesh.point(_point1);
}

AX_INLINE
bool EditableMesh::Edge::hasPoint(PointId p0, PointId p1) {
	return (p0 == _point0 && p1 == _point1)
		|| (p1 == _point0 && p0 == _point1);
}

AX_INLINE
EditableMesh::EdgeId& EditableMesh::Edge::next(PointId p) {
	if (_point0 == p) return _next0;
	AX_ASSERT(_point1 == p);
	return _next1;
}

AX_INLINE
EditableMesh::Point& EditableMesh::FaceEdge::point(Mesh& mesh) {
	return mesh.point(_point);
}

AX_INLINE
EditableMesh::Vec3& EditableMesh::FaceEdge::normal(Mesh& mesh) {
	return mesh.fvNormal(_id);
}

AX_INLINE
EditableMesh::Edge& EditableMesh::FaceEdge::edge(Mesh& mesh) {
	return mesh.edge(_edge);
}

template<class CH> inline
void EditableMesh::Edge::onFormat(Format_<CH>& f) const {
	f.format("[edge {?}, next={?},{?} v={?},{?}]"
		, ax_enum_int(_id)
		, ax_enum_int(_next0), ax_enum_int(_next1)
		, ax_enum_int(_point0), ax_enum_int(_point1));
}

}// namespace
