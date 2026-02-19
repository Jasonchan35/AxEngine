module;

export module AxCore.LinearMath:Shapes;
export import :Mat;

export namespace ax::Math {
#define AX_MathAxisDir_ENUM_LIST(E) \
	E(X,) \
	E(Y,) \
	E(Z,) \
//---
AX_ENUM_CLASS_NO_STR(AX_MathAxisDir_ENUM_LIST, AxisDir, u8)
} export namespace ax {
AX_ENUM_STR_UTIL(AX_MathAxisDir_ENUM_LIST, Math::AxisDir);
}export namespace ax::Math {

template<class T>
class Ray3_;

template<class T>
class Sphere3_ {
	using Vec3 = Vec3_<T>;
public:
	constexpr Sphere3_() = default;
	constexpr Sphere3_(const Vec3& center_, const T& radius_) : center(center_), radius(radius_) {}
	
	template<class CH>
	void onFormat(Format_<CH>& f) const;

	Vec3	center;
	T		radius;
};

template<class T>
class Triangle3_ {
	using Vec3 = Vec3_<T>;
public:
	constexpr Triangle3_() = default;
	constexpr Triangle3_(const Vec3& v0_, const Vec3& v1_, const Vec3& v2_) : v0(v0_), v1(v1_), v2(v2_) {}

	Vec3 v0, v1, v2;
};

template<class T>
class Plane3_ {
	using Vec3 = Vec3_<T>;
	using Vec4 = Vec4_<T>;
public:
	constexpr Plane3_() = default;
	constexpr Plane3_(const Vec3& normal_, T distance_)
		: normal(normal_)
		, distance(distance_) 
	{}

	constexpr Plane3_(const Vec3& normal_, const Vec3& point_)
		: normal(normal_)
		, distance(normal_.dot(point_))
	{}

	constexpr Plane3_(const Triangle3_<T>&  tri) {
		normal = (tri.v1 - tri.v0).cross(tri.v2 - tri.v0).normalize();
		distance = normal.dot(tri.v0);
	}

	template<class CH>
	void onFormat(Format_<CH>& f) const;

	Vec4 toVec4() const { return Vec4(normal, distance); }
	
	Vec3	normal;
	T		distance; // distance from origin
};

template<class T>
class Cylinder3_ {
	using Vec3 = Vec3_<T>;
public:
	constexpr Cylinder3_() = default;
	constexpr Cylinder3_(const Vec3& start_, const Vec3& end_, const T& radius_)
		: start(start_)
		, end(end_)
		, radius(radius_)
	{}

	Vec3	start;
	Vec3	end;
	T		radius;
};

template<class T>
class Capsule3_ {
	using Vec3 = Vec3_<T>;
public:
	constexpr Capsule3_() = default;
	constexpr Capsule3_(const Vec3& start_, const Vec3& end_, const T& radius_)
		: start(start_)
		, end(end_)
		, radius(radius_)
	{}

	Vec3	start;
	Vec3	end;
	T		radius;
};


template<class T>
class Line2_ {
	using Vec2 = Vec2_<T>;
	using Vec3 = Vec3_<T>;
public:
	constexpr Line2_() = default;
	constexpr Line2_(const Vec2& start_, const Vec2& end_)
		: start(start_)
		, end(end_)
	{}

	constexpr bool getClosestPoint(Vec2& outPoint, const Vec2& inPoint) const;
	constexpr T	distanceToPoint(const Vec2& pt) const;

	Vec2	start;
	Vec2	end;
};

template<class T>
class Line3_ {
	using Vec3 = Vec3_<T>;
	using Ray3 = Ray3_<T>;
public:
	constexpr Line3_() = default;
	constexpr Line3_(const Vec3& start_, const Vec3& end_)
		: start(start_)
		, end(end_)
	{}

	constexpr bool getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const;
	constexpr bool getClosestPoint(Vec3& outPoint, const Ray3& ray    ) const;

	constexpr T	distanceToPoint(const Vec3& pt) const;

	Vec3	start;
	Vec3	end;
};


template<class T>
class Ray3_ {
	using Vec2		= Vec2_<T>;
	using Vec3		= Vec3_<T>;
	using Vec4		= Vec4_<T>;
	using Mat4		= Mat4_<T>;
	using Quat4		= Quat4_<T>;
	using Rect2		= Rect2_<T>;
	using Line3		= Line3_<T>;
public:

	Vec3 origin, dir;

	AX_META_TYPE(Ray3_, NoBaseClass) {
		AX_META_FIELD(origin) {};
		AX_META_FIELD(dir) {};
	};
	
	constexpr Ray3_() = default;
	constexpr Ray3_(const Vec3& origin_, const Vec3& dir_) : origin(origin_), dir(dir_) {}

	constexpr static Ray3_ s_zero() { return Ray3_(Vec3(0), Vec3(0)); }

	constexpr static Ray3_ s_unprojectFromInverseMatrix(const Vec2& screenPos, const Mat4& invMat, const Rect2& viewport);

	struct HitTestResult {
		T		distance = infinity();
		Vec3	point;
		Vec3	normal;

		bool	hasResult() const { return distance != infinity_<T>(); }
	};

	constexpr bool getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const;
	constexpr bool getClosestPoint(Vec3& outPoint, const Line3& line, T minOnLine = 0, T maxOnLine = 1) const;
};

using Ray3f = Ray3_<f32>;
using Ray3d = Ray3_<f64>;

using Sphere3f = Sphere3_<f32>;
using Sphere3d = Sphere3_<f64>;

using Plane3f  = Plane3_<f32>;
using Plane3d  = Plane3_<f64>;

using Triangle3f = Triangle3_<f32>;
using Triangle3d = Triangle3_<f64>;

using Capsule3f = Capsule3_<f32>;
using Capsule3d = Capsule3_<f64>;

using Line2f = Line2_<f32>;
using Line2d = Line2_<f64>;

using Line3f = Line3_<f32>;
using Line3d = Line3_<f64>;

template<class T> constexpr
Ray3_<T> Ray3_<T>::s_unprojectFromInverseMatrix(const Vec2& screenPos, const Mat4& invMat, const Rect2& viewport) {
	auto pt = screenPos;

	Vec3 p0(pt, 0);
	Vec3 p1(pt, 1);

	auto v0 = invMat.unprojectPointFromInverseMatrix(p0, viewport);
	auto v1 = invMat.unprojectPointFromInverseMatrix(p1, viewport);

	Ray3_ o;
	o.origin = v0;
	o.dir = (v1 - v0).normalize();
	return o;
}

template<class T> constexpr
bool Ray3_<T>::getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const {
	auto v = inPoint - origin;
	auto t = v.dot(dir);
	outPoint = (t <= 0) ? origin : origin + dir * t;
	return true;
}

template<class T> constexpr
bool Ray3_<T>::getClosestPoint(Vec3& outPoint, const Line3_<T>& line, T minOnLine, T maxOnLine) const {
	auto da = line.end - line.start;
	auto db = dir;
	auto dc = origin - line.start;

	auto ab = da.cross(db);
	if (ab.equals0())
		return false;

	auto s = dc.cross(db).dot( da.cross(db) ) / ab.lengthSq();
	s = ax_clamp<T>(s, minOnLine, maxOnLine);
	outPoint = line.start + da * s;
	return true;
}


template<class T>
template<class CH> inline
void Sphere3_<T>::onFormat(Format_<CH>& f) const {
	f << Fmt("Sphere[center={}, radius={}", center, radius);
}

template<class T>
template<class CH> inline
void Plane3_<T>::onFormat(Format_<CH>& f) const {
	f << Fmt("Plane[normal={}, distance={}]", normal, distance);
}

template<class T> constexpr
bool Line2_<T>::getClosestPoint(Vec2& outPoint, const Vec2& inPoint) const {
	Line3_<T> line3(start.xy0(), end.xy0());
	Vec3 closestPt;
	if (!line3.getClosestPoint(closestPt, inPoint.xy0()))
		return false;

	outPoint = closestPt.xy();
	return true;
}

template<class T> constexpr
T Line2_<T>::distanceToPoint(const Vec2& pt) const {
	Vec2 closestPt;
	if (getClosestPoint(closestPt, pt)) {
		return closestPt.distance(pt);
	}
	return infinity_<T>();
}

template<class T> constexpr
bool Line3_<T>::getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const {
	auto line = end - start;
	auto len = line.length();

	if (almostZero(len))
		return false;

	auto u = line / len; //normalized line
	auto v = inPoint - start;

	auto w = u.dot(v);
	w = ax_clamp(w, T(0), len);

	outPoint = start + u * w;
	return true;
}

template<class T> constexpr
bool Line3_<T>::getClosestPoint(Vec3& outPoint, const Ray3& ray) const {
	auto da = end - start;
	auto db = ray.dir;
	auto dc = ray.origin - start;

	auto ab = da.cross(db);
	if (ab.equals0())
		return false;

	auto s = dc.cross(db).dot( da.cross(db) ) / ab.lengthSq();
	s = ax_clamp01(s);
	outPoint = start + da * s;
	return true;
}

template<class T> constexpr 
T Line3_<T>::distanceToPoint(const Vec3& pt) const {
	Vec3 closestPt;
	if (getClosestPoint(closestPt, pt)) {
		return closestPt.distance(pt);
	}
	return infinity_<T>();
}

} // namespace
