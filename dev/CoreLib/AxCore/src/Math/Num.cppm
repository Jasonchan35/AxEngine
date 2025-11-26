module;
#include "AxCore-pch.h"
export module AxCore.Num;
export import AxCore._PCH;

export namespace ax {

#define AX_NumSIMD_ENUM_LIST(E) \
	E(NoSIMD,) \
	E(SSE,)  \
//---
AX_ENUM_CLASS(AX_NumSIMD_ENUM_LIST, CpuSIMD, u8)

// template<Int N, class T, class STORAGE> class NumBase_;
// template<       class T, class STORAGE> using Num1_  = NumBase_<1, T, STORAGE>;
// template<       class T, class STORAGE> using Num2_  = NumBase_<2, T, STORAGE>;
// template<       class T, class STORAGE> using Num3_  = NumBase_<3, T, STORAGE>;
// template<       class T, class STORAGE> using Num4_  = NumBase_<4, T, STORAGE>;

template<Int N, class T, CpuSIMD SIMD> class Num_Storage;

template<Int N, class T, class STORAGE>
class NumBase_ : public STORAGE {
	using This = NumBase_;
protected:
	using STORAGE::_data;
public:
	using Storage = STORAGE;
	using Element = STORAGE::Element;
	static constexpr Int elementCount = STORAGE::elementCount;
	static constexpr CpuSIMD cpuSIMD = STORAGE::cpuSIMD;
	
	      T* data()			{ return _data; }
	const T* data() const	{ return _data; }
	
	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_data); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_data); }

	AX_INLINE constexpr NumBase_() = default;
	AX_INLINE constexpr NumBase_(const T& e0, const T& e1, const T& e2) : Storage(e0, e1, e2) {
		static_assert(elementCount == 3);
	}
	AX_INLINE constexpr NumBase_(const T& e0, const T& e1, const T& e2, const T& e3) : Storage(e0, e1, e2, e3) {
		static_assert(elementCount == 4);
	} 
	
	AX_INLINE constexpr This operator+(const This& rhs) const {
		This tmp; for (Int i = 0; i < elementCount; ++i) { tmp._data[i] = this->_data[i] + rhs._data[i]; }
		return tmp;
	}
};

//----------------

inline constexpr CpuSIMD Vec_DefaultSIMD = CpuSIMD::SSE;
template<Int N, class T, CpuSIMD SIMD> class Vec_Storage_; 

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

template<>
class Vec_Storage_<3, f32, CpuSIMD::SSE> {
	using T = f32;
public:
	using Element = T;
	static constexpr Int elementCount = 3;
	static constexpr CpuSIMD cpuSIMD = CpuSIMD::NoSIMD;
	
	union {
		T _data[elementCount];
		struct { T _e0, _e1, _e2, _e3_unused; };
		struct { T x, y, z; };
		__m128 _m;
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

template<>
class Vec_Storage_<4, f32, CpuSIMD::SSE> {
	using T = f32;
public:
	using Element = T;
	static constexpr Int elementCount = 4;
	static constexpr CpuSIMD cpuSIMD = CpuSIMD::SSE;
	union {
		T _data[elementCount];
		struct { T _e0, _e1, _e2, _e3; };
		struct { T x, y, z, w; };
		__m128 _m;
	};
	
	AX_INLINE constexpr Vec_Storage_() = default;
	AX_INLINE constexpr Vec_Storage_(const T& e0, const T& e1, const T& e2, const T& e3) : _e0(e0), _e1(e1), _e2(e2), _e3(e3) {}
};

template<Int N, class T, CpuSIMD SIMD>
class VecBase_ : public NumBase_<N, T, Vec_Storage_<N, T, SIMD> > {
	using This = VecBase_;
	using Base = NumBase_<N, T, Vec_Storage_<N, T, SIMD>>;
public:
	using Storage = Base::Storage;
	using Element = Base::Element;
	static constexpr Int elementCount = Base::elementCount;
	static constexpr CpuSIMD cpuSIMD  = Base::cpuSIMD;
	
	AX_INLINE constexpr VecBase_() = default;
	AX_INLINE constexpr VecBase_(const T& e0, const T& e1, const T& e2) : Base(e0, e1, e2) {}
	AX_INLINE constexpr VecBase_(const T& e0, const T& e1, const T& e2, const T& e3) : Base(e0, e1, e2, e3) {}
}; 

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

template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec3_ = VecBase_<3, T, SIMD>;
template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec4_ = VecBase_<4, T, SIMD>;

using Vec3f			= Vec3_<f32>;
using Vec3f_SSE		= Vec3_<f32, CpuSIMD::SSE>;
using Vec3f_NoSIMD	= Vec3_<f32, CpuSIMD::NoSIMD>;

using Vec4f			= Vec4_<f32>;
using Vec4f_SSE		= Vec4_<f32, CpuSIMD::SSE>;
using Vec4f_NoSIMD	= Vec4_<f32, CpuSIMD::NoSIMD>;

} // namespace
