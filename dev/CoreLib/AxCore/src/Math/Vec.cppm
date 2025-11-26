module;
#include "AxCore-pch.h"
export module AxCore.Vec;
export import AxCore.NumSIMD4;

export namespace  ax {

inline constexpr CpuSIMD Vec_DefaultSIMD = CpuSIMD::SSE;
template<Int N, class T, CpuSIMD SIMD> class Vec_Storage_;

template<Int N, class T, CpuSIMD SIMD> class Vec_;
template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec3_ = Vec_<3, T, SIMD>;
template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec4_ = Vec_<4, T, SIMD>;

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

template<Int N, class T, CpuSIMD SIMD> using Vec_NumSIMD = NumSIMD_<N,  Vec_<N, T, SIMD>, Vec_Storage_<N, T, SIMD> >;

template<class T, CpuSIMD SIMD>
class Vec_<3, T, SIMD> : public Vec_NumSIMD<3, T, SIMD> {
	using Base = Vec_NumSIMD<3, T, SIMD>;
	using This = Vec_;
public:
	using Storage = Base::Storage;
	using Element = typename Storage::Element;
	static constexpr Int elementCount = Storage::elementCount;
	static constexpr CpuSIMD cpuSIMD  = Storage::cpuSIMD;
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(Tag::All_, const T& v_) : Base(Tag::All, v_) {}
	AX_INLINE constexpr Vec_(const Storage & storage) : Base(storage) {} 
	AX_INLINE constexpr Vec_(const T& e0, const T& e1, const T& e2) : Base(e0, e1, e2) {}
	AX_INLINE constexpr Vec_(const T& e0, const T& e1, const T& e2, const T& e3) : Base(e0, e1, e2, e3) {}
};


template<class T, CpuSIMD SIMD>
class Vec_<4, T, SIMD> : public Vec_NumSIMD<4, T, SIMD> {
	using Base = Vec_NumSIMD<4, T, SIMD>;
	using This = Vec_;
public:
	using Storage = Base::Storage;
	using Element = typename Storage::Element;
	static constexpr Int elementCount = Storage::elementCount;
	static constexpr CpuSIMD cpuSIMD  = Storage::cpuSIMD;
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(Tag::All_, const T& v_) : Base(Tag::All, v_) {}
	AX_INLINE constexpr Vec_(const Storage & storage) : Base(storage) {} 
	AX_INLINE constexpr Vec_(const T& e0, const T& e1, const T& e2) : Base(e0, e1, e2) {}
	AX_INLINE constexpr Vec_(const T& e0, const T& e1, const T& e2, const T& e3) : Base(e0, e1, e2, e3) {}
};

}