module;
#include "AxCore-pch.h"
export module AxCore.Vec;
export import AxCore.NumSIMD4;

export namespace  ax {

inline constexpr CpuSIMD Vec_DefaultSIMD = CpuSIMD::SSE;
template<Int N, class T, CpuSIMD SIMD> class Vec_Storage_;

template<Int N, class T, CpuSIMD SIMD> class Vec_;
template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec1_ = Vec_<1, T, SIMD>;
template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec2_ = Vec_<2, T, SIMD>;
template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec3_ = Vec_<3, T, SIMD>;
template<class T, CpuSIMD SIMD = Vec_DefaultSIMD> using Vec4_ = Vec_<4, T, SIMD>;

using Vec3f			= Vec3_<f32>;
using Vec3f_SSE		= Vec3_<f32, CpuSIMD::SSE>;
using Vec3f_Basic	= Vec3_<f32, CpuSIMD::None>;

using Vec4f			= Vec4_<f32>;
using Vec4f_SSE		= Vec4_<f32, CpuSIMD::SSE>;
using Vec4f_Basic	= Vec4_<f32, CpuSIMD::None>;

template<class T, CpuSIMD SIMD>
class Vec_Storage_<1, T, SIMD> {
	using REG = CpuSIMD_REG_<1, T, SIMD>;
	using MM  = typename REG::Type;
public:
	using Element = T;
	static constexpr Int elementCount = 1;
	static constexpr CpuSIMD cpuSIMD = SIMD;
	
	union {
		T _data[elementCount];
		struct { T _e0; };
		struct { T x; };
		MM _m;
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
		T _data[elementCount];
		struct { T _e0, _e1; };
		struct { T x, y; };
		MM _m;
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
		T _data[elementCount + REG::padding];
		struct { T _e0, _e1, _e2; };
		struct { T x, y, z; };
		MM _m;
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
		T _data[elementCount];
		struct { T _e0, _e1, _e2, _e3; };
		struct { T x, y, z, w; };
		MM _m;
	};
	
	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_() = default;
	AX_NODISCARD AX_INLINE explicit	constexpr Vec_Storage_(const MM& m) : _m(m) {}
	AX_NODISCARD AX_INLINE			constexpr Vec_Storage_(const T& e0, const T& e1, const T& e2, const T& e3) : _e0(e0), _e1(e1), _e2(e2), _e3(e3) {}
};

template<Int N, class T, CpuSIMD SIMD> using VecBase_ = NumSIMD_<N,  Vec_<N, T, SIMD>, Vec_Storage_<N, T, SIMD> >;

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