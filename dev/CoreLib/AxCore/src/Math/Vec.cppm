module;
#include "AxCore-pch.h"
export module AxCore.Vec;
export import AxCore.NumSIMD4;

export namespace  ax {

inline constexpr CpuSIMD Vec_DefaultSIMD = CpuSIMD::SSE;
template<Int N, class T, CpuSIMD SIMD> class Vec_Storage_;
template<Int N, class T, CpuSIMD SIMD> class Vec_;
template<Int N, class T, CpuSIMD SIMD> using VecBase_ = NumSIMD_<N,  Vec_<N, T, SIMD>, Vec_Storage_<N, T, SIMD> >;

template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec1_ = Vec_<1, T, SIMD>;
template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec2_ = Vec_<2, T, SIMD>;
template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec3_ = Vec_<3, T, SIMD>;
template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec4_ = Vec_<4, T, SIMD>;

using Vec1h			= Vec1_<f16>;
using Vec1h_SSE		= Vec1_<f16, CpuSIMD::SSE>;
using Vec1h_Basic	= Vec1_<f16, CpuSIMD::None>;
using Vec2h			= Vec2_<f16>;
using Vec2h_SSE		= Vec2_<f16, CpuSIMD::SSE>;
using Vec2h_Basic	= Vec2_<f16, CpuSIMD::None>;
using Vec3h			= Vec3_<f16>;
using Vec3h_SSE		= Vec3_<f16, CpuSIMD::SSE>;
using Vec3h_Basic	= Vec3_<f16, CpuSIMD::None>;
using Vec4h			= Vec4_<f16>;
using Vec4h_SSE		= Vec4_<f16, CpuSIMD::SSE>;
using Vec4h_Basic	= Vec4_<f16, CpuSIMD::None>;

using Vec1f			= Vec1_<f32>;
using Vec1f_SSE		= Vec1_<f32, CpuSIMD::SSE>;
using Vec1f_Basic	= Vec1_<f32, CpuSIMD::None>;
using Vec2f			= Vec2_<f32>;
using Vec2f_SSE		= Vec2_<f32, CpuSIMD::SSE>;
using Vec2f_Basic	= Vec2_<f32, CpuSIMD::None>;
using Vec3f			= Vec3_<f32>;
using Vec3f_SSE		= Vec3_<f32, CpuSIMD::SSE>;
using Vec3f_Basic	= Vec3_<f32, CpuSIMD::None>;
using Vec4f			= Vec4_<f32>;
using Vec4f_SSE		= Vec4_<f32, CpuSIMD::SSE>;
using Vec4f_Basic	= Vec4_<f32, CpuSIMD::None>;

using Vec1d			= Vec1_<f64>;
using Vec1d_SSE		= Vec1_<f64, CpuSIMD::SSE>;
using Vec1d_Basic	= Vec1_<f64, CpuSIMD::None>;
using Vec2d			= Vec2_<f64>;
using Vec2d_SSE		= Vec2_<f64, CpuSIMD::SSE>;
using Vec2d_Basic	= Vec2_<f64, CpuSIMD::None>;
using Vec3d			= Vec3_<f64>;
using Vec3d_SSE		= Vec3_<f64, CpuSIMD::SSE>;
using Vec3d_Basic	= Vec3_<f64, CpuSIMD::None>;
using Vec4d			= Vec4_<f64>;
using Vec4d_SSE		= Vec4_<f64, CpuSIMD::SSE>;
using Vec4d_Basic	= Vec4_<f64, CpuSIMD::None>;

using Vec1i			= Vec1_<Int>;
using Vec1i_SSE		= Vec1_<Int, CpuSIMD::SSE>;
using Vec1i_Basic	= Vec1_<Int, CpuSIMD::None>;
using Vec2i			= Vec2_<Int>;
using Vec2i_SSE		= Vec2_<Int, CpuSIMD::SSE>;
using Vec2i_Basic	= Vec2_<Int, CpuSIMD::None>;
using Vec3i			= Vec3_<Int>;
using Vec3i_SSE		= Vec3_<Int, CpuSIMD::SSE>;
using Vec3i_Basic	= Vec3_<Int, CpuSIMD::None>;
using Vec4i			= Vec4_<Int>;
using Vec4i_SSE		= Vec4_<Int, CpuSIMD::SSE>;
using Vec4i_Basic	= Vec4_<Int, CpuSIMD::None>;

template<class T, CpuSIMD SIMD>
class Vec_Storage_<1, T, SIMD> {
	using REG = CpuSIMD_REG_<1, T, SIMD>;
	using MM  = typename REG::Type;
public:
	using Element = T;
	static constexpr Int elementCount = 1;
	static constexpr CpuSIMD cpuSIMD = SIMD;
	
	union {
		MM _m;
		T _data[elementCount];
		struct { T _e0; };
		struct { T x; };
	};

	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_() = default;
	AX_NODISCARD AX_INLINE explicit	constexpr Vec_Storage_(const MM& m) : _m(m) {}
	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_(const T& e0) : _e0(e0) {}
};

template<class T, CpuSIMD SIMD>
class Vec_Storage_<2, T, SIMD> {
	using REG = CpuSIMD_REG_<2, T, SIMD>;
	using MM  = typename REG::Type;
public:
	using Element = T;
	static constexpr Int elementCount = 2;
	static constexpr CpuSIMD cpuSIMD = SIMD;
	
	union {
		MM _m;
		T _data[elementCount];
		struct { T _e0, _e1; };
		struct { T x, y; };
	};

	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_() = default;
	AX_NODISCARD AX_INLINE explicit	constexpr Vec_Storage_(const MM& m) : _m(m) {}
	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_(const T& e0, const T& e1) : _e0(e0), _e1(e1) {}
};

template<class T, CpuSIMD SIMD>
class Vec_Storage_<3, T, SIMD> {
	using REG = CpuSIMD_REG_<3, T, SIMD>;
	using MM  = typename REG::Type;
public:
	using Element = T;
	static constexpr Int elementCount = 3;
	static constexpr CpuSIMD cpuSIMD = SIMD;
	
	union {
		MM _m;
		T _data[elementCount + REG::padding];
		struct { T _e0, _e1, _e2; };
		struct { T x, y, z; };
	};

	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_() = default;
	AX_NODISCARD AX_INLINE explicit	constexpr Vec_Storage_(const MM& m) : _m(m) {}
	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_(const T& e0, const T& e1, const T& e2) : _e0(e0), _e1(e1), _e2(e2) {}
};

template<class T, CpuSIMD SIMD>
class Vec_Storage_<4, T, SIMD> {
	using REG = CpuSIMD_REG_<4, T, SIMD>;
	using MM  = typename REG::Type;
public:
	using Element = T;
	static constexpr Int elementCount = 4;
	static constexpr CpuSIMD cpuSIMD = SIMD;
	union {
		MM _m;
		T _data[elementCount];
		struct { T _e0, _e1, _e2, _e3; };
		struct { T x, y, z, w; };
	};
	
	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_() = default;
	AX_NODISCARD AX_INLINE explicit	constexpr Vec_Storage_(const MM& m) : _m(m) {}
	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_(const T& e0, const T& e1, const T& e2, const T& e3) : _e0(e0), _e1(e1), _e2(e2), _e3(e3) {}
};

template<class T, CpuSIMD SIMD>
class Vec_<1, T, SIMD> : public VecBase_<1, T, SIMD> {
	using Base = VecBase_<1, T, SIMD>;
	using This = Vec_;
public:
	using Storage = typename Base::Storage;
	using Element = typename Base::Element;
	static_assert(Type_IsSame<T, Element>);
	static constexpr Int     elementCount = Base::elementCount;
	static constexpr CpuSIMD cpuSIMD      = Base::cpuSIMD;
	static_assert(SIMD == cpuSIMD);
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(Tag::All_, const T& v) : Base(Tag::All, v) {}
	AX_INLINE constexpr Vec_(const Storage & storage) : Base(storage) {}
	AX_INLINE constexpr Vec_(const T& e0) : Base(e0) {}
};

template<class T, CpuSIMD SIMD>
class Vec_<2, T, SIMD> : public VecBase_<2, T, SIMD> {
	using Base = VecBase_<2, T, SIMD>;
	using This = Vec_;
public:
	using Storage = typename Base::Storage;
	using Element = typename Base::Element;
	static_assert(Type_IsSame<T, Element>);
	static constexpr Int     elementCount = Base::elementCount;
	static constexpr CpuSIMD cpuSIMD      = Base::cpuSIMD;
	static_assert(SIMD == cpuSIMD);
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const This& v) = default;	
	AX_INLINE constexpr Vec_(Tag::All_, const T& v) : Base(Tag::All, v) {}
	AX_INLINE constexpr Vec_(const Storage & storage) : Base(storage) {}
	AX_INLINE constexpr Vec_(const T& e0, const T& e1) : Base(e0, e1) {}
};

template<class T, CpuSIMD SIMD>
class Vec_<3, T, SIMD> : public VecBase_<3, T, SIMD> {
	using Base = VecBase_<3, T, SIMD>;
	using This = Vec_;
public:
	using Storage = typename Base::Storage;
	using Element = typename Base::Element;
	static_assert(Type_IsSame<T, Element>);
	static constexpr Int     elementCount = Base::elementCount;
	static constexpr CpuSIMD cpuSIMD      = Base::cpuSIMD;
	static_assert(SIMD == cpuSIMD);
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(Tag::All_, const T& v) : Base(Tag::All, v) {}
	AX_INLINE constexpr Vec_(const Storage & storage) : Base(storage) {}
	AX_INLINE constexpr Vec_(const T& e0, const T& e1, const T& e2) : Base(e0, e1, e2) {}
};

template<class T, CpuSIMD SIMD>
class Vec_<4, T, SIMD> : public VecBase_<4, T, SIMD> {
	using Base = VecBase_<4, T, SIMD>;
	using This = Vec_;
public:
	using Storage = typename Base::Storage;
	using Element = typename Base::Element;
	static_assert(Type_IsSame<T, Element>);
	static constexpr Int     elementCount = Base::elementCount;
	static constexpr CpuSIMD cpuSIMD      = Base::cpuSIMD;
	static_assert(SIMD == cpuSIMD);
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(Tag::All_, const T& v) : Base(Tag::All, v) {}
	AX_INLINE constexpr Vec_(const Storage & storage) : Base(storage) {}
	AX_INLINE constexpr Vec_(const T& e0, const T& e1, const T& e2, const T& e3) : Base(e0, e1, e2, e3) {}
};



}