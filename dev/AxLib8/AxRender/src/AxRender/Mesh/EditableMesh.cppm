module;

export module AxRender:EditableMesh;
export import :VertexLayout;

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

	//-----------------------------------------------------------------
	struct Point : public NonCopyable {
		Vec3		pos;
		Vec3		normal;
		PointId	id() const { return _id; }

	friend class EditableMesh;
	protected:
		Int			_faceCount = 0;
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
		PointId		pointId () const { return _pointId; }
		EdgeId		edgeId () const { return _edgeId; }
		FaceId		faceId () const { return _faceId; }

	friend class EditableMesh;
	protected:
		FaceEdgeId		_id			= FaceEdgeId::Invalid;
		PointId			_pointId	= PointId::Invalid;
		EdgeId			_edgeId		= EdgeId::Invalid;
		FaceId			_faceId		= FaceId::Invalid;
		FaceEdgeId		_next		= FaceEdgeId::Invalid;
		FaceEdgeId		_adjacent	= FaceEdgeId::Invalid; //FaceEdge shared by the same edge
	};

	//-----------------------------------------------------------------
	struct Face : public NonCopyable {
		Vec3		normal;
		Vec3		center;
		Int			pointCount() const { return _faceEdgeCount; }
		FaceId		id() const { return _id; }

		void				getPoints	(Mesh& mesh, IArray<Point*>& result) const;
		void				getPoints	(const Mesh& mesh, IArray<const Point*>& result) const;
		void				getPositions(const Mesh& mesh, IArray<Vec3>& result) const;
		
		MutSpan<FaceEdge>	getFaceEdges(Mesh& mesh) const;
		MutSpan<Vec3>		getNormals	(Mesh& mesh) const;
		MutSpan<Color>		getColors	(Mesh& mesh, Int colorChannelId) const;
		MutSpan<Vec2>		getUVs		(Mesh& mesh, Int uvChannelId) const;
		MutSpan<Vec4>		getCustoms	(Mesh& mesh, Int customChannelId) const;

		Span<FaceEdge>		getFaceEdges(const Mesh& mesh) const { return getFaceEdges(ax_const_cast(mesh)); }
		Span<Vec3>			getNormals	(const Mesh& mesh) const { return getNormals(ax_const_cast(mesh)); }
		Span<Color>			getColors	(const Mesh& mesh, Int colorChannelId ) const { return getColors(ax_const_cast(mesh), colorChannelId); }
		Span<Vec2>			getUVs		(const Mesh& mesh, Int uvChannelId    ) const { return getUVs(ax_const_cast(mesh), uvChannelId); }
		Span<Vec4>			getCustoms	(const Mesh& mesh, Int customChannelId) const { return getCustoms(ax_const_cast(mesh), customChannelId); }
		
	friend class EditableMesh;
	protected:
		
		FaceId			_id   = FaceId::Invalid;
		FaceEdgeId		_faceEdgeHead = FaceEdgeId::Invalid;
		Int				_faceEdgeCount;
	};

	//-----------------------------------------------------------------
	template<class T>
	struct FaceVertexChannel : public NonCopyable {
		T			defaultValue;
		NameId		name;
		Array<T>	values;
	};
	struct UVChannel : public FaceVertexChannel<Vec2> {
	};
	struct ColorChannel : public FaceVertexChannel<Color> {
	};
	struct CustomChannel : public FaceVertexChannel<Vec4> {
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

	Int colorChannelCount() const { return _fvColorChannels.size(); }
	ColorChannel& addColorChannel(const Color & defaultValue);

	Int uvChannelCount() const { return _fvUVChannels.size(); }
	UVChannel& addUvChannel(const Vec2& defaultValue);

	Int customChannelCount() const { return _fvCustomChannels.size(); }
	CustomChannel& addCustomChannel(const Vec4& defaultValue);

	using NormalCount = VertexLayout::NormalCount;

	EditableMesh();

	Array<Face>          _faces;
	Array<Point>         _points;
	Array<Edge>          _edges;
	Array<FaceEdge>      _faceEdges;
	Array<Vec3>          _fvNormals;
	Array<ColorChannel>  _fvColorChannels;
	Array<UVChannel>     _fvUVChannels;
	Array<CustomChannel> _fvCustomChannels;
	DisplayOptions       _displayOptions;
};

AX_INLINE
MutSpan<EditableMesh::Vec3> EditableMesh::Face::getNormals(Mesh& mesh) const {
	return mesh._fvNormals.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
MutSpan<EditableMesh::Color> EditableMesh::Face::getColors(Mesh& mesh, Int colorChannelId) const {
	return mesh._fvColorChannels[colorChannelId].values.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
MutSpan<EditableMesh::Vec2> EditableMesh::Face::getUVs(Mesh& mesh, Int uvChannelId) const {
	return mesh._fvUVChannels[uvChannelId].values.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
MutSpan<EditableMesh::Vec4> EditableMesh::Face::getCustoms(Mesh& mesh, Int customChannelId) const {
	return mesh._fvCustomChannels[customChannelId].values.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
MutSpan<EditableMesh::FaceEdge> EditableMesh::Face::getFaceEdges(Mesh& mesh) const {
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
	return mesh.point(_pointId);
}

AX_INLINE
EditableMesh::Vec3& EditableMesh::FaceEdge::normal(Mesh& mesh) {
	return mesh.fvNormal(_id);
}

AX_INLINE
EditableMesh::Edge& EditableMesh::FaceEdge::edge(Mesh& mesh) {
	return mesh.edge(_edgeId);
}

template<class CH> inline
void EditableMesh::Edge::onFormat(Format_<CH>& f) const {
	f.format("[edge {?}, next={?},{?} v={?},{?}]"
		, ax_enum_int(_id)
		, ax_enum_int(_next0), ax_enum_int(_next1)
		, ax_enum_int(_point0), ax_enum_int(_point1));
}

}// namespace
