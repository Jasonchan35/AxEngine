module;
#include "AxCore-pch.h"

export module AxCore.NumTuple;

export import AxCore.CpuSIMD;
export import AxCore.FixedSpan;
export import AxCore.Format;

export namespace ax {

constexpr CpuSIMD NumTuple_SIMD = CpuSIMD::SSE;

// forward declare
template<Int N, class T, CpuSIMD SIMD = NumTuple_SIMD> class NumTuple_;

template<class T, CpuSIMD SIMD = NumTuple_SIMD> using NumTuple1_ = NumTuple_<1, T, SIMD>;
template<class T, CpuSIMD SIMD = NumTuple_SIMD> using NumTuple2_ = NumTuple_<2, T, SIMD>;
template<class T, CpuSIMD SIMD = NumTuple_SIMD> using NumTuple3_ = NumTuple_<3, T, SIMD>;
template<class T, CpuSIMD SIMD = NumTuple_SIMD> using NumTuple4_ = NumTuple_<4, T, SIMD>;

template<class T, CpuSIMD SIMD>
class NumTuple_<1, T, SIMD> {
	using This = NumTuple_;
public:
	using Element = T;
	static constexpr Int kElementCount = 1;

	T	x;
	
	AX_INLINE constexpr NumTuple_() = default;
	AX_INLINE constexpr NumTuple_( const Element& x_) : x(x_) {}
	AX_INLINE constexpr void set(const Element& x_) { x = x_; }

	AX_INLINE 		Element*	data()		 { return &x; }
	AX_INLINE const	Element*	data() const { return &x; }
	
	AX_NODISCARD AX_INLINE constexpr bool operator==(const This& r) const	{ return x == r.x; }
	AX_NODISCARD AX_INLINE constexpr bool operator!=(const This& r) const	{ return x != r.x; }

	using CSpan =    Span<Element>;
	using MSpan = MutSpan<Element>;

	using CFixedSpan =    FixedSpan<Element, kElementCount>;
	using MFixedSpan = MutFixedSpan<Element, kElementCount>;

	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }
};

} // namespace

