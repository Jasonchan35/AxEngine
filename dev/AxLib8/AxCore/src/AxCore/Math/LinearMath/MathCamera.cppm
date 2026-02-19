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
	using Plane3 = Plane3_<T>;
	
	T     fieldOfView = T(50.0); // vertical
	T     nearClip = T(0.1);
	T     farClip  = T(10000.0);
	
	Rect2 viewport {0,0,1920,1080};
	
	Ray3 getRay(const Vec2& screenPos, const ProjectionDesc& desc) const;
	
	FixedArray<Vec3,   8> getFrustumPoints(const ProjectionDesc& desc) const;
	FixedArray<Plane3, 6> getFrustumPlanes(const ProjectionDesc& desc) const;

	Mat4 projMatrix(const ProjectionDesc& desc) const;
};

template<class T>
class ViewportCamera3_: public Camera3_<T> {
	using Base = Camera3_<T>;
	using Vec2  = Vec2_<T>;
	using Vec3  = Vec3_<T>;
	using Vec4  = Vec4_<T>;
	using Mat4  = Mat4_<T>;
	using Quat4 = Quat4_<T>;
	using Rect2 = Rect2_<T>;
	using Ray3	= Ray3_<T>;
public:
	Vec3  aim {0, 0, 0};
	Quat4 rotation { Quat4::s_identity() };
	T     distance = 100;
	

	Vec3 eye() const { return aim + rotation * Vec3(0, 0, -distance); }
	void setEye(const Vec3& eye);

	void setRotation(const Vec3& v) { rotation.setEulerDeg(v); }
	void setRotation(const T& x, const T& y, const T& z = 0) { setRotation(Vec3(x, y, z)); }

	Vec3 right() const		{ return rotation * Vec3(1, 0, 0); }
	Vec3 up() const			{ return rotation * Vec3(0, 1, 0); }
	Vec3 forward() const	{ return rotation * Vec3(0, 0, 1); }

	void pan	(Vec2 delta);
	void orbit	(Vec2 delta);
	void move	(const Vec2& delta) { move(Vec3(delta, 0)); }
	void move	(const Vec3& delta);

	void dolly	(T delta);
	
	Mat4 worldMatrix(const ProjectionDesc& desc) const;
	Mat4 viewMatrix(const ProjectionDesc& desc) const;
	Mat4 viewProjMatrix(const ProjectionDesc& desc) const { return Base::projMatrix(desc) * viewMatrix(desc); }
};

using Camera3f = Camera3_<f32>;
using Camera3d = Camera3_<f64>;

using BoomCamera3f = ViewportCamera3_<f32>;
using BoomCamera3d = ViewportCamera3_<f64>;

} // namespace
