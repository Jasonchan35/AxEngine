module;

export module AxCore.LinearMath:Camera;
export import :Shapes;

export namespace ax::Math {

template<class T>
class Camera3_ {
public:
	using Vec2  = Vec2_<T>;
	using Vec3  = Vec3_<T>;
	using Vec4  = Vec4_<T>;
	using Mat4  = Mat4_<T>;
	using Quat4 = Quat4_<T>;
	using Rect2 = Rect2_<T>;
	using Ray3	= Ray3_<T>;

	void pan	(Vec2 delta);
	void orbit	(Vec2 delta);
	void move	(const Vec2& delta) { move(Vec3(delta, 0)); }
	void move	(const Vec3& delta);

	void dolly	(T delta);

	Vec3 eye() const { return aim + rotation * Vec3(0, 0, -distance); }
	void setEye(const Vec3& eye) {
		auto dir = eye - aim;
		distance = Math::max(dir.length(), T(0.001));
		rotation = Quat4::s_lookAt(dir / distance, up());
	}
	
	void setRotation(const Vec3& v) { rotation.setEulerDeg(v); }
	void setRotation(const T& x, const T& y, const T& z = 0) { setRotation(Vec3(x, y, z)); }

	Vec3 right() const		{ return rotation * Vec3(1, 0, 0); }
	Vec3 up() const			{ return rotation * Vec3(0, 1, 0); }
	Vec3 forward() const	{ return rotation * Vec3(0, 0, 1); }

	Ray3 getRay(const Vec2& screenPos, const ProjectionDesc& desc) const;

	Mat4 viewMatrix(const ProjectionDesc& desc) const;
	Mat4 projMatrix(const ProjectionDesc& desc) const;
	Mat4 viewProjMatrix(const ProjectionDesc& desc) const { return projMatrix(desc) * viewMatrix(desc); }

	Vec3  aim {0, 0, 0};
	Quat4 rotation { Quat4::s_identity() };
	T     distance = 100;

	T     fieldOfView = T(50.0); // vertical
	T     nearClip = T(0.1);
	T     farClip  = T(10000.0);
	
	Rect2 viewport {0,0,1920,1080};
};

using Camera3f = Camera3_<f32>;
using Camera3d = Camera3_<f64>;

template<class T> inline
void Camera3_<T>::pan(Vec2 delta) {
	auto d = rotation * Vec3(0, 0, distance);
	rotation *= Quat4::s_eulerRad({delta, 0});
	aim = d - rotation * Vec3(0, 0, distance);
}

template<class T> inline
void Camera3_<T>::orbit(Vec2 delta) {
	auto e = rotation.eulerRad();
	e.x = 0;
	auto q = Quat4::s_eulerRad(e);

	rotation *= q.inverse() * Quat4::s_eulerRadX(delta.x) * q;
	rotation *= Quat4::s_eulerRadY(delta.y);
}

template<class T> inline
void Camera3_<T>::move(const Vec3& delta) {
	aim += rotation * delta;
}

template<class T> inline
void Camera3_<T>::dolly(T delta) {
	distance += delta;
}

template<class T> inline
Ray3_<T> Camera3_<T>::getRay(const Vec2& screenPos, const ProjectionDesc& desc) const {
	return Ray3::s_unprojectFromInverseMatrix(screenPos, viewProjMatrix(desc).inverse(), viewport);
}


template<class T> inline
Mat4_<T> Camera3_<T>::projMatrix(const ProjectionDesc& desc) const {
	return Mat4::s_perspective(radians(fieldOfView),
	                           viewport.w, viewport.h, nearClip, farClip,
	                           desc);
}

template<class T> inline
Mat4_<T> Camera3_<T>::viewMatrix(const ProjectionDesc& desc) const {
	return Mat4::s_lookAt(eye(), aim, up(), desc);
}

} // namespace
