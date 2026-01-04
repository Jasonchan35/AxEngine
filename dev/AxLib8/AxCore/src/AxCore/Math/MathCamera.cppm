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

	void pan	(T x, T y);
	void orbit	(T x, T y);
	void move	(T x, T y, T z);
	void move	(const Vec3& v) { move(v.x, v.y, v.z); }

	void dolly	(T z);

	void setPos(T x, T y, T z)	{ setPos(Vec3(x,y,z)); }
	void setAim(T x, T y, T z)	{ setAim(Vec3(x,y,z)); }
	void setUp (T x, T y, T z)	{ setUp (Vec3(x,y,z)); }

	void setPos(const Vec3& pos)	{ _pos = pos; }
	void setAim(const Vec3& aim)	{ _aim = aim; }
	void setUp (const Vec3& up)		{ _up  = up;  }

	const Vec3& pos() const { return _pos; }
	const Vec3& aim() const { return _aim; }
	const Vec3& up () const { return _up;  }

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
	Vec3  _pos{150, 150, 200};
	Vec3  _aim{0, 0, 0};
	Vec3  _up{0, 1, 0};
};

using Camera3f = Camera3_<f32>;
using Camera3d = Camera3_<f64>;

template<class T>
void Camera3_<T>::pan(T x, T y) {
	auto v = _aim - _pos;
	auto right = _up.cross(v.normal());

	auto q = Quat4::s_eulerY(x) * Quat4::s_angleAxis(y, right);
	v   *= q;
	_up *= q;
	_aim = _pos + v;
}

template<class T>
void Camera3_<T>::orbit(T x, T y) {
	auto v = _pos - _aim;
	auto right = _up.cross(v.normal());

	auto q = Quat4::s_eulerY(x) * Quat4::s_angleAxis(y, right);
	v   *= q;
	_up *= q;
	_pos = _aim + v;
}

template<class T>
void Camera3_<T>::move(T x, T y, T z) {
	auto v = _aim - _pos;
	auto dir = v.normal();
	auto right = _up.cross(dir);

	auto t = right * x + _up * y + dir * z;
	_pos += t;
	_aim += t;
}

template<class T>
void Camera3_<T>::dolly(T z) {
	auto v = _pos - _aim;
	auto dir = v.normal();
	auto d = v.length();
	d += z;
	ax_max_it(d, static_cast<T>(0.001));
	
	_pos = _aim + dir * d;
}

template<class T>
Ray3_<T> Camera3_<T>::getRay(const Vec2& screenPos) const {
	return Ray3::s_unProjectFromInvMatrix(screenPos, viewProjMatrix().inverse(), _viewport);
}

template<class T>
Mat4_<T> Camera3_<T>::viewMatrix() const {
	return Mat4::s_lookAt(_pos, _aim, _up);
}

template<class T>
Mat4_<T> Camera3_<T>::projMatrix() const {
	T aspect = _viewport.h != 0 ? _viewport.w / _viewport.h : T(0);
	return Mat4::s_perspective(radians(_fov), aspect, _nearClip, _farClip);
}

} // namespace

