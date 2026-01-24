module AxCore.LinearMath;
import :Camera;
import :Quat;

namespace ax::Math {

template<class T>
Mat4_<T> Camera3_<T>::viewMatrix(const ProjectionDesc& desc) const {
#if 0
	if (desc.isRightHanded) {
		auto eye = aim + rotation * Vec3(0, 0, distance);
		return Mat4::s_lookAt(eye, aim, up(), desc);
	} else {
		auto eye = aim + rotation * Vec3(0, 0, -distance);
		return Mat4::s_lookAt(eye, aim, up(), desc);
	}
	
#else
	if (desc.isRightHanded) {
		auto pos = Vec3(0, 0, distance) - rotation.conjugate() * aim;
		return Mat4::s_TRS(pos, rotation, Vec3::s_one());
	} else {
		auto pos = Vec3(0, 0, -distance) - rotation.conjugate() * aim;
		return Mat4::s_TRS(pos, rotation, Vec3::s_one());
	}
#endif
}

// ---- explicit instantiation --- 
template class Camera3_<f32>;
template class Camera3_<f64>;

} // namespace


