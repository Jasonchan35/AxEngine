module;
#include "AxCore-pch.h"
export module AxCore.Vec;
export import AxCore.Num;

export namespace  ax {

inline constexpr CpuSIMD Vec_DefaultSIMD = CpuSIMD::SSE;
template<Int N, class T, CpuSIMD SIMD> class Vec_Storage_;

template<Int N, class T, CpuSIMD SIMD> class VecBase_;
template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec3_ = VecBase_<3, T, SIMD>;
template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec4_ = VecBase_<4, T, SIMD>;

using Vec3f			= Vec3_<f32>;
using Vec3f_SSE		= Vec3_<f32, CpuSIMD::SSE>;
using Vec3f_NoSIMD	= Vec3_<f32, CpuSIMD::NoSIMD>;

using Vec4f			= Vec4_<f32>;
using Vec4f_SSE		= Vec4_<f32, CpuSIMD::SSE>;
using Vec4f_NoSIMD	= Vec4_<f32, CpuSIMD::NoSIMD>;


template<class T>
class Vec_Storage_<3, T, CpuSIMD::NoSIMD> {
public:
	using Element = T;
	static constexpr Int elementCount = 3;
	static constexpr CpuSIMD cpuSIMD = CpuSIMD::NoSIMD;
	
	union {
		T _data[elementCount];
		struct { T _e0, _e1, _e2; };
		struct { T x, y, z; };
	};

	AX_INLINE constexpr Vec_Storage_() = default;
	AX_INLINE constexpr Vec_Storage_(const T& e0, const T& e1, const T& e2) : _e0(e0), _e1(e1), _e2(e2) {}
};

template<class T>
class Vec_Storage_<4, T, CpuSIMD::NoSIMD> {
public:
	using Element = T;
	static constexpr Int elementCount = 4;
	static constexpr CpuSIMD cpuSIMD = CpuSIMD::NoSIMD;
	
	union {
		T _data[elementCount];
		struct { T _e0, _e1, _e2, _e3; };
		struct { T x, y, z, w; };
	};

	AX_INLINE constexpr Vec_Storage_() = default;
	AX_INLINE constexpr Vec_Storage_(const T& e0, const T& e1, const T& e2, const T& e3) : _e0(e0), _e1(e1), _e2(e2), _e3(e3) {}
};

//---- SSE

template<>
class Vec_Storage_<3, f32, CpuSIMD::SSE> {
	using T = f32;
	static constexpr bool _is_f64 = Type_IsSame<T, f64>;
	using MM = std::conditional_t<_is_f64, __m256d, __m128>;
public:
	using Element = T;
	static constexpr Int elementCount = 3;
	static constexpr CpuSIMD cpuSIMD = CpuSIMD::NoSIMD;
	
	union {
		T _data[elementCount];
		struct { T _e0, _e1, _e2, _e3_unused; };
		struct { T x, y, z; };
		MM _m;
	};

	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_() = default;
	AX_NODISCARD AX_INLINE explicit	constexpr Vec_Storage_(const MM& m) : _m(m) {}
	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_(const T& e0, const T& e1, const T& e2) : _e0(e0), _e1(e1), _e2(e2) {}
};

template<class T>
class Vec_Storage_<4, T, CpuSIMD::SSE> {
	static constexpr bool _is_f64 = Type_IsSame<T, f64>;
	using MM = std::conditional_t<_is_f64, __m256d, __m128>;
public:
	using Element = T;
	static constexpr Int elementCount = 4;
	static constexpr CpuSIMD cpuSIMD = CpuSIMD::SSE;
	union {
		T _data[elementCount];
		struct { T _e0, _e1, _e2, _e3; };
		struct { T x, y, z, w; };
		MM _m;
	};
	
	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_() = default;
	AX_NODISCARD AX_INLINE explicit	constexpr Vec_Storage_(const MM& m) : _m(m) {}
	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_(const T& e0, const T& e1, const T& e2, const T& e3) : _e0(e0), _e1(e1), _e2(e2), _e3(e3) {}
};

template<Int N, class T, CpuSIMD SIMD>
using VecBase_Base = Num_<N,  VecBase_<N, T, SIMD>, Vec_Storage_<N, T, SIMD> >;

template<class T, CpuSIMD SIMD>
class VecBase_<4, T, SIMD> : public VecBase_Base<4, T, SIMD> {
	using This = VecBase_;
	using Base = Num_<4, VecBase_, Vec_Storage_<4, T, SIMD>>;
public:
	using _NumLimit = typename NumBase_NumLimit<This, T>;

	using Storage = Base::Storage;
	using Element = typename Storage::Element;
	static constexpr Int elementCount = Storage::elementCount;
	static constexpr CpuSIMD cpuSIMD  = Storage::cpuSIMD;
	
	AX_INLINE constexpr VecBase_() = default;
	AX_INLINE constexpr VecBase_(Tag::All_, const T& v_) : Base(Tag::All, v_) {}
	AX_INLINE constexpr VecBase_(const Storage & storage) : Base(storage) {} 
	AX_INLINE constexpr VecBase_(const T& e0, const T& e1, const T& e2) : Base(e0, e1, e2) {}
	AX_INLINE constexpr VecBase_(const T& e0, const T& e1, const T& e2, const T& e3) : Base(e0, e1, e2, e3) {}
};


namespace Math {
// for Unit Test
template <Int N, class T, CpuSIMD A_SIMD, CpuSIMD B_SIMD> requires (A_SIMD != B_SIMD)
inline constexpr bool almostEqual(const VecBase_<N, T, A_SIMD>& a,
                                     const VecBase_<N, T, B_SIMD>& b) {
	auto ep = Math::epsilon<T>;
	for (Int i = 0; i < N; ++i) {
		if (!almostEqual(a.unsafe_at(i), b.unsafe_at(i), ep)) return false;
	}
	return true;
}
} // namespace Math


// template<class T, CpuSIMD SIMD = Vec_DefaultSIMD>
// class Vec3_ : public VecBase_<3, T, SIMD> {
// public:
// 	AX_INLINE constexpr Vec3_() = default;
// }; 

// template<class T, CpuSIMD SIMD = Vec_DefaultSIMD>
// class Vec4_ : public VecBase_<4, T, SIMD> {
// 	using Base = VecBase_<4, T, SIMD>;
// public:
// 	AX_INLINE constexpr Vec4_() = default;
// 	AX_INLINE constexpr Vec4_(const T& e0, const T& e1, const T& e2, const T& e3) : Base(e0, e1, e2, e3) {}
// };

} // namespace