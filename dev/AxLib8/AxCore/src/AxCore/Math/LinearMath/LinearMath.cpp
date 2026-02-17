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

// ---- explicit instantiation --- 
template class Camera3_<f32>;
template class Camera3_<f64>;

} // namespace


