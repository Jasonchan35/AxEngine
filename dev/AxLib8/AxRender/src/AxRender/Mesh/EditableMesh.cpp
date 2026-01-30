module;

module AxRender;
import :EditableMesh;

namespace ax {
// Face::_faceEdgeHead
// Face::_faceEdgeTail
// 
//    head -->  0 ----> 1
//              ^       |
//              |       |
//    tail -->  3 <---- 2
// 
// Point::_edgeHead, _edgeTail also doing the way, but the Edge::_next0, _next1 if determined by Edge::_point0 == Point::_id

void EditableMesh::clear() {
	_points.clear();
	_edges.clear();
	_faces.clear();
	_faceEdges.clear();

	_fvNormals.clear();
	_fvUVChannels.clear();
	_fvColorChannels.clear();
	_fvCustomChannels.clear();
}

EditableMesh::Point& EditableMesh::addPoint(const Vec3& pos) {
	auto pointId = static_cast<PointId>(_points.size());
	auto& p = _points.emplaceBack();
	p._id = pointId;
	p.pos = pos;
	return p;
}

 EditableMesh::EdgeId EditableMesh::findEdge(PointId a, PointId b) {
	auto head = point(a)._edgeHead;
	if (head == EdgeId::Invalid) {
		return EdgeId::Invalid;
	}

	auto edgeId = head;
	do {	
		auto& e = edge(edgeId);
		if (e.hasPoint(a,b)) {
			return edgeId;
		}
		edgeId = e.next(a);
	}while (edgeId != head);

	return EdgeId::Invalid;
}

EditableMesh::Face& EditableMesh::addFace(IntSpan pointIndices) {
	auto  faceId		 = static_cast<FaceId>(_faces.size());
	auto  faceEdgeBaseId = static_cast<FaceEdgeId>(_faceEdges.size());
	auto& face			 = _faces.emplaceBack();
	face._id = faceId;

	auto n = pointIndices.size();
	if (!n) return face;

	face._faceEdgeHead = faceEdgeBaseId;
	face._faceEdgeCount = n;

	auto prevSize = _faceEdges.size();
	_faceEdges.incSize(n);
	auto faceEdges = _faceEdges.sliceFrom(prevSize);

	{
		auto faceVertexCount = _faceEdges.size();

		_fvNormals.resize(faceVertexCount);

		for (auto& s : _fvColorChannels) {
			s.values.resize(faceVertexCount, s.defaultValue);
		}

		for (auto& s : _fvUVChannels) {
			s.values.resize(faceVertexCount, s.defaultValue);
		}

		for (auto& s : _fvCustomChannels) {
			s.values.resize(faceVertexCount, s.defaultValue);
		}
	}

	Int i = 0;
	for (auto& fe : faceEdges) {
		auto vi0 = static_cast<PointId>(pointIndices[i]);
		auto vi1 = static_cast<PointId>(pointIndices[(i+1) % n]);

		auto faceEdgeId = faceEdgeBaseId + i;
		fe._id = faceEdgeId;
		fe._next = faceEdgeBaseId + (i + 1) % n;
		fe._face = faceId;
		fe._point = vi0;

		auto& point0 = point(vi0);
		auto& point1 = point(vi1);

		auto edgeId = findEdge(vi0, vi1);
		if (edgeId == EdgeId::Invalid) {
			//new edge
			edgeId = static_cast<EdgeId>(_edges.size());
			auto& edge = _edges.emplaceBack();
			edge._id = edgeId;
			edge.addFaceEdge(*this, fe);
			edge.addToPoint(*this, point0, point1);
		}else{
			auto& e = edge(edgeId);
			e.addFaceEdge(*this, fe);
		}

		++i;
	}

	return face;
}

void EditableMesh::updateFaceNormals() {
	Array<Point*, 64> points;

	for (auto& p : _points) {
		p.normal.set(0,0,0);
	}

	for (auto& e : _edges) {
		e.normal.set(0,0,0);
	}

	for (auto& face : _faces) {
		face.getPoints(*this, points);

		Vec3 normal(0,0,0);
		Vec3 center(0,0,0);

		Int n = points.size();
		if (n >= 3) {
			auto v0 = points[0]->pos;
			center += v0;

			for (Int i = 1; i < n-1; i++) {
				auto v1 = points[i]->pos;
				auto v2 = points[i+1]->pos;
				center += v1;
				normal += (v1 - v0).cross(v2 - v0).normalize();
			}
			center += points.back()->pos;

			//----
			center /= static_cast<double>(n);
			normal /= static_cast<double>(n - 2);
		}

		face.center = center;
		face.normal = normal.normalize();

		//------
		auto faceEdges = face.getFaceEdges(*this);
		for (auto& fe : faceEdges) {
			point(fe._point).normal += face.normal;
			edge(fe._edge).normal += face.normal;
		}
	}

	for (auto& v : _points) {
		v.normal.normalizeSelf();
	}

	for (auto& e : _edges) {
		e.normal.normalizeSelf();
	}
}

void EditableMesh::updateFaceVertexNormals(double hardEdgeAngleDeg) {
	auto cosAngle = Math::cos(Math::radians(hardEdgeAngleDeg * 2));

	_fvNormals.resize(_faceEdges.size());
	for (auto& nl : _fvNormals) {
		nl.set(0,0,0);
	}

	for (auto& e : _edges) {
		auto& f0 = face(faceEdge(e._faceEdgeHead)._face);
		auto& f1 = face(faceEdge(e._faceEdgeTail)._face);
		e.softEdge = Math::abs(f0.normal.dot(f1.normal)) > cosAngle;
	}

	for (auto& f : _faces) {
		auto  faceEdges  = f.getFaceEdges(*this);
		auto  outNormals = f.getNormals(*this);
		auto* outN       = outNormals.begin();

		for (auto& fe0 : faceEdges) {
			auto& fe1 = faceEdge(fe0._next);
			auto& e0 = edge(fe0._edge);
			auto& e1 = edge(fe1._edge);

			if (e0.softEdge && e1.softEdge) {
				*outN = point(fe0._point).normal;
			}else{
				*outN = face(fe0._face).normal;
			}
			outN++;
		}

		AX_ASSERT(outN == outNormals.end());
	}
}

auto EditableMesh::addColorChannel(const Color & defaultValue) -> ColorChannel& {
	auto& ch = _fvColorChannels.emplaceBack();
	ch.defaultValue = defaultValue;
	ch.values.resize(_faceEdges.size(), defaultValue);
	return ch;
}

auto EditableMesh::addUvChannel(const Vec2& defaultValue) -> UVChannel& {
	auto& ch = _fvUVChannels.emplaceBack();
	ch.defaultValue = defaultValue;
	ch.values.resize(_faceEdges.size(), defaultValue);
	return ch;
}

auto EditableMesh::addCustomChannel(const Vec4& defaultValue) -> CustomChannel& {
	auto& ch = _fvCustomChannels.emplaceBack();
	ch.defaultValue = defaultValue;
	ch.values.resize(_faceEdges.size(), defaultValue);
	return ch;
}

EditableMesh::EditableMesh() {
}

void EditableMesh::Face::getPoints(Mesh& mesh, IArray<Point*>& result) const {
	result.resize(_faceEdgeCount);
	auto* fv = result.data();
	auto t = _faceEdgeHead;

	for (Int i = 0; i < _faceEdgeCount; i++) {
		auto& fe = mesh.faceEdge(t + i);
		*fv = &mesh.point(fe._point);
		fv++;
	}
	AX_ASSERT(fv == result.end());
}

void EditableMesh::Face::getPositions(const Mesh& mesh, IArray<Vec3>& result) const {
	result.resize(_faceEdgeCount);
	auto* fv = result.data();
	auto t = _faceEdgeHead;

	for (Int i = 0; i < _faceEdgeCount; i++) {
		auto& fe = mesh.faceEdge(t + i);
		*fv = mesh.point(fe._point).pos;
		fv++;
	}
	AX_ASSERT(fv == result.end());
}

void EditableMesh::Edge::addFaceEdge(Mesh& mesh, FaceEdge& fe) {
	AX_ASSERT(_id != EdgeId::Invalid);
	fe._edge = _id;
	if (_faceEdgeHead == FaceEdgeId::Invalid) {
		_faceEdgeHead = fe._id;
		_faceEdgeTail = fe._id;
		fe._adjacent  = fe._id;
	} else {
		auto& tail = mesh.faceEdge(_faceEdgeTail);
		tail._adjacent = fe._id;
		_faceEdgeTail  = fe._id;
		fe._adjacent   = _faceEdgeHead;
	}
}

auto EditableMesh::Edge::center(Mesh& mesh) -> Vec3 {
	auto p0 = mesh.point(_point0).pos;
	auto p1 = mesh.point(_point1).pos;
	return (p0 + p1) * 0.5;
}

void EditableMesh::Edge::addToPoint(Mesh& mesh, Point& p0, Point& p1) {
	AX_ASSERT(_point0 == PointId::Invalid);
	AX_ASSERT(_point1 == PointId::Invalid);

	_point0 = p0._id;
	_point1 = p1._id;

	if (p0._edgeHead == EdgeId::Invalid) {
		p0._edgeHead = _id;
	}else{
		auto& tail = mesh.edge(p0._edgeTail);
		tail.next(_point0) = _id;
	}

	if (p1._edgeHead == EdgeId::Invalid) {
		p1._edgeHead = _id;
	}else{
		auto& tail = mesh.edge(p1._edgeTail);
		tail.next(_point1) = _id;
	}

	p0._edgeTail = _id;
	p1._edgeTail = _id;

	_next0 = p0._edgeHead;
	_next1 = p1._edgeHead;

	p0._faceCount++;
	p1._faceCount++;
}

} // namespace
