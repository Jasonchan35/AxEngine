module;

export module AxCore.MathCamera;
export import AxCore.MathShapes;

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

	void setAim(const Vec3& aim)	{ _aim = aim; }
	void setPos(const Vec3& pos) {
		auto dir = pos - _aim;
		_distance = dir.length();
		_quat = Quat4::lookAt(dir / _distance, up());
	}
	
	const Vec3& aim() const { return _aim; }
	
	const Vec3 pos() const { return _aim + _quat * Vec3(0,0,-_distance); }

	const Vec3 up() const		{ return _quat * Vec3(0, 1, 0); }
	const Vec3 forward() const	{ return _quat * Vec3(0, 0, 1); }
	const Vec3 right() const	{ return _quat * Vec3(0, 1, 0); }

	void setViewport(const Rect2& v) { _viewport = v; }
	const Rect2& viewport() const { return _viewport; }

	Ray3	getRay(const Vec2& screenPos) const;

	Mat4	viewMatrix() const;
	Mat4	projMatrix() const;
	Mat4	viewProjMatrix() const { return projMatrix() * viewMatrix(); }

private:
	T     _fov      = T(50.0);
	T     _nearClip = T(0.1);
	T     _farClip  = T(10000.0);
	Rect2 _viewport {0,0,1920,1080};
	Vec3  _aim{0, 0, 0};
	
	// Vec3  _pos{50, 50, 100};
	// Vec3  _up{0, 1, 0};
	
	T     _distance = 20;
	Quat4 _quat { Quat4::s_identity() };
};

using Camera3f = Camera3_<f32>;
using Camera3d = Camera3_<f64>;

template<class T> inline
void Camera3_<T>::pan(Vec2 delta) {
	auto d = _quat * Vec3(0, 0, -_distance);
	_quat *= Quat4::s_euler({delta, 0});
	_aim = _quat * Vec3(0, 0, _distance);
}

template<class T> inline
void Camera3_<T>::orbit(Vec2 delta) {
	_quat *= Quat4::s_euler({0, delta.x, delta.y});
}

template<class T> inline
void Camera3_<T>::move(const Vec3& delta) {
	_aim += _quat * Vec3(delta.x, delta.y, 0) + Vec3(0,0,delta.z);
}

template<class T> inline
void Camera3_<T>::dolly(T delta) {
	_distance += delta;
}

template<class T> inline
Ray3_<T> Camera3_<T>::getRay(const Vec2& screenPos) const {
	return Ray3::s_unProjectFromInvMatrix(screenPos, viewProjMatrix().inverse(), _viewport);
}

template<class T> inline
Mat4_<T> Camera3_<T>::viewMatrix() const {
	return Mat4::s_translate(Vec3(0, 0, -_distance)) * _quat.to_Mat4();
}

template<class T> inline
Mat4_<T> Camera3_<T>::projMatrix() const {
	if (Math::almostZero(_viewport.h)) {
		AX_ASSERT(false);
		return {};
	}
	T aspect = _viewport.w / _viewport.h;
	return Mat4::s_perspective(radians(_fov), aspect, _nearClip, _farClip);
}

} // namespace

