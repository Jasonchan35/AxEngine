module;

export module AxCore.Margin;
export import AxCore.Vec;

export namespace ax {

template<class T, VecSIMD SIMD = VecSIMD_Default> class Margin2_;
using Margin2h = Margin2_<f16>;
using Margin2f = Margin2_<f32>;
using Margin2d = Margin2_<f64>;
using Margin2i = Margin2_<Int>;
using Margin2s = Margin2_<i16>;


template<class T, VecSIMD SIMD>
class Margin2_ {
	using This = Margin2_;
	static constexpr Int N = 4;
public:
	using Vec2	= Vec2_<T, SIMD>;

	using _NumLimit = VecSIMD_NumLimit<This, T>;
	using ElementType = T;
	static constexpr Int kElementCount = N;
	static constexpr VecSIMD kVecSIMD = SIMD;

	using Num4 = Num4_<T>;
		
	using SIMD_Data = VecSIMD_Data_<N,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T top, right, bottom, left; };
	};	

	constexpr Margin2_() = default;
	constexpr Margin2_(const SIMD_Data & simd) : _simd(simd) {}
	constexpr Margin2_(AxTag::Zero_) : _simd(AxTag::Zero) {}
	constexpr Margin2_(T v) : _simd(SIMD_Data::s_all(v)) {}
	constexpr Margin2_(T x, T y) : Margin2_(Vec2(x,y)) { }
	constexpr Margin2_(const Vec2& vec) : _simd(vec.y, vec.x, vec.y, vec.x) { }
	constexpr Margin2_(T top_, T right_, T bottom_, T left_) : _simd(top_, right_, bottom_, left_) {}
	
	constexpr void set(T v)									{ _simd = SIMD_Data(v,v,v,v); }
	constexpr void set(T x, T y)								{ _simd = SIMD_Data(y,x,y,x); }
	constexpr void set(T top_, T right_, T bottom_, T left_)	{ _simd = SIMD_Data(top_,right_,bottom_,left_); }

	AX_NODISCARD static constexpr This s_zero() { return SIMD_Data::s_zero(); }

	AX_NODISCARD constexpr This	inflate(Vec2 v) const { return This(top - v.y, right + v.x, bottom + v.y, left - v.x); }
	AX_NODISCARD constexpr This	deflate(Vec2 v) const { return inflate(-v); }

	AX_NODISCARD constexpr This	operator-() const { return This(-top, -right, -bottom, -left); }

	AX_NODISCARD constexpr Vec2	total	() const { return Vec2(totalX(), totalY()); }
	AX_NODISCARD constexpr T	totalX	() const { return left + right;  }
	AX_NODISCARD constexpr T	totalY	() const { return top  + bottom; }

	AX_NODISCARD constexpr Vec2	topLeft		() const { return Vec2(left,  top   ); }
	AX_NODISCARD constexpr Vec2	topRight	() const { return Vec2(right, top   ); }
	AX_NODISCARD constexpr Vec2	bottomLeft	() const { return Vec2(left,  bottom); }
	AX_NODISCARD constexpr Vec2	bottomRight	() const { return Vec2(right, bottom); }

	template<VecSIMD R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec) const { return _simd.almostEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool almostZero(  const This& rhs) const { return _simd.almostZero(rhs._simd); }
	AX_NODISCARD AX_INLINE constexpr bool exactlyEqual(const This& vec) const { return _simd.exactlyEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool operator==(  const This& vec) const { return _simd == vec._simd; }
	
	AX_NODISCARD constexpr This operator+ (const T&    rhs) const { return _simd + rhs; }
	AX_NODISCARD constexpr This operator- (const T&    rhs) const { return _simd - rhs; }
	AX_NODISCARD constexpr This operator* (const T&    rhs) const { return _simd * rhs; }
	AX_NODISCARD constexpr This operator/ (const T&    rhs) const { return _simd / rhs; }
	AX_NODISCARD constexpr This operator+ (const Vec2& rhs) const { return _simd + This(rhs)._simd; }
	AX_NODISCARD constexpr This operator- (const Vec2& rhs) const { return _simd - This(rhs)._simd; }
	AX_NODISCARD constexpr This operator* (const Vec2& rhs) const { return _simd * This(rhs)._simd; }
	AX_NODISCARD constexpr This operator/ (const Vec2& rhs) const { return _simd / This(rhs)._simd; }
	AX_NODISCARD constexpr This operator+ (const This& rhs) const { return _simd + rhs._simd; }
	AX_NODISCARD constexpr This operator- (const This& rhs) const { return _simd - rhs._simd; }
	AX_NODISCARD constexpr This operator* (const This& rhs) const { return _simd * rhs._simd; }
	AX_NODISCARD constexpr This operator/ (const This& rhs) const { return _simd / rhs._simd; }
};

} // namespace
