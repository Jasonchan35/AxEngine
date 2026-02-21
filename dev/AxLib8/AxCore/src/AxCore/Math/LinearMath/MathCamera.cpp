module AxCore.LinearMath;
import :Camera;
import :Quat;
import :Mat;

import AxCore.Logger;

namespace ax::Math {

template<class T> inline
Ray3_<T> Camera3_<T>::getRay(const Vec2& screenPos, const ProjectionDesc& desc) const {
	return Ray3::s_unprojectFromInverseMatrix(screenPos, projMatrix(desc).inverse(), viewport);
}

template<class T> inline
Mat4_<T> Camera3_<T>::projMatrix(const ProjectionDesc& desc) const {
	return Mat4::s_perspective(radians(fieldOfView),
							   viewport.w, viewport.h, nearClip, farClip,
							   desc);
}

template<class T>
auto Camera3_<T>::getFrustumPoints(const ProjectionDesc& desc, const Mat4& worldMatrix) const -> FixedArray<Vec3, 8> {
	//     4-------5
	//    /|      /|
	//   0-------1 |
	//   | 7-----|-6
	//   |/      |/
	//   3-------2

	auto invMat = projMatrix(desc).inverse();
	T zNear = desc.isReverseZ ? T(1) : T(0);
	T zFar  = desc.isReverseZ ? T(0) : T(1);

	auto func = [&](const Vec3& s) -> Vec3 {
		return worldMatrix.mulPoint4x3(-invMat.mulPoint(s));
	};
	
	FixedArray<Vec3, 8> o;
	o[0] = func(Vec3(-1,-1, zNear));
	o[1] = func(Vec3( 1,-1, zNear));
	o[2] = func(Vec3( 1, 1, zNear));
	o[3] = func(Vec3(-1, 1, zNear));

	o[4] = func(Vec3(-1,-1, zFar));
	o[5] = func(Vec3( 1,-1, zFar));
	o[6] = func(Vec3( 1, 1, zFar));
	o[7] = func(Vec3(-1, 1, zFar));
	
	return o;
}

template<class T>
auto Camera3_<T>::getFrustumPlanes(const ProjectionDesc& desc, const Mat4& worldMatrix) const -> FixedArray<Plane3, 6> {
	auto points = getFrustumPoints(desc, worldMatrix);
	FixedArray<Plane3, 6> o;
	
	auto func = [&](const Vec3& v0, const Vec3& v1, const Vec3& v2) -> Plane3 {
		return Plane3(Triangle3_<T>(v0, v1, v2));
	};
	
	o[0] = func(points[0], points[1], points[2]); // near
	o[1] = func(points[4], points[7], points[6]); // far
	o[2] = func(points[0], points[3], points[7]); // left
	o[3] = func(points[1], points[5], points[6]); // right
	o[4] = func(points[0], points[4], points[5]); // top
	o[5] = func(points[2], points[6], points[7]); // bottom
	return o;
}


template<class T>
void ViewportCamera3_<T>::pan(Vec2 delta) {
	auto pivot = eye();
	rotation = Quat4::s_eulerY(delta.y) * Quat4::s_angleAxis(delta.x, right()) * rotation;
	aim = pivot + rotation * Vec3(0, 0, distance);
}

template<class T>
void ViewportCamera3_<T>::orbit(Vec2 delta) {
	rotation = Quat4::s_eulerY(delta.y) * Quat4::s_angleAxis(delta.x, right()) * rotation;
}

template<class T>
void ViewportCamera3_<T>::roll(T delta) {
	rotation = Quat4::s_angleAxis(delta, forward()) * rotation;
}

template<class T>
void ViewportCamera3_<T>::setEye(const Vec3& eye) {
	auto dir = eye - aim;
	distance = Math::max(dir.length(), T(0.001));
	rotation = Quat4::s_lookAt(dir / distance, up());
}

template<class T> inline
void ViewportCamera3_<T>::move(const Vec3& delta) {
	aim += rotation * delta;
}

template<class T> inline
void ViewportCamera3_<T>::dolly(T delta) {
	distance = Math::max(T(0.001), distance + delta);
}

template<class T>
Mat4_<T> ViewportCamera3_<T>::worldMatrix(const ProjectionDesc& desc) const {
	// return viewMatrix(desc).inverse();
	return Mat4::s_TRS(eye(), rotation, Vec3::s_one());
}

template<class T> inline
Mat4_<T> ViewportCamera3_<T>::viewMatrix(const ProjectionDesc& desc) const {
	return Mat4::s_lookAt(eye(), aim, up(), desc);
}

// ---- explicit instantiation --- 
template class Camera3_<f32>;
template class Camera3_<f64>;

template class ViewportCamera3_<f32>;
template class ViewportCamera3_<f64>;

} // namespace


