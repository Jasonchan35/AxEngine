module AxCore.LinearMath;
import :Camera;
import :Quat;
import :Mat;

namespace  ax {

//	template class  Mat_<4,4,f32, VecSimd_Default>;
//	template class  Mat_<4,4,f32, VecSimd::SSE>;
//	template class  Mat_<4,4,f32, VecSimd::Basic>;
//
//	template class  Mat_<4,4,f64, VecSimd_Default>;
//	template class  Mat_<4,4,f64, VecSimd::SSE>;
//	template class  Mat_<4,4,f64, VecSimd::Basic>;
} // namespace
 
namespace ax::Math {

template<class T>
auto Camera3_<T>::getFrustumPoints(const ProjectionDesc& desc) const -> FixedArray<Vec3, 8> {
	auto invMat = viewProjMatrix(desc).inverse();
	
	T zNear = desc.isReverseZ ? T(1) : T(0);
	T zFar  = desc.isReverseZ ? T(0) : T(1);

	auto func = [&](const Vec4& s) -> Vec3 {
		return invMat.mulPoint(s).xyz_div_w();
	};
	
	FixedArray<Vec3, 8> o;
	o[0] = func(Vec4(-1,-1, zNear, 1));
	o[1] = func(Vec4( 1,-1, zNear, 1));
	o[2] = func(Vec4( 1, 1, zNear, 1));
	o[3] = func(Vec4(-1, 1, zNear, 1));

	o[4] = func(Vec4(-1,-1, zFar, 1));
	o[5] = func(Vec4( 1,-1, zFar, 1));
	o[6] = func(Vec4( 1, 1, zFar, 1));
	o[7] = func(Vec4(-1, 1, zFar, 1));
	
	return o;
}

// ---- explicit instantiation --- 
template class Camera3_<f32>;
template class Camera3_<f64>;

} // namespace


