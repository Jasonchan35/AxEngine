module;
export module AxCore.Num;
export import AxCore.MetaType;
export import AxCore.Float16;
export import AxCore.NormInt;

export namespace ax {

template<class A, class B>
concept CON_IsSame = std::is_same_v<A, B>;

template<Int COL, Int ROW, class T> struct Num_Data {
protected:
	T _data[ROW][COL];
};

template<class T> struct Num_Data<1,1,T> {
	union {
		struct { T x; };
		T _data[1][1];
	};
};

template<class T> struct Num_Data<2,1,T> {
	union {
		struct { T x,y; };
		T _data[1][2];
	};
};

template<class T> struct Num_Data<3,1,T> {
	union {
		struct { T x,y,z; };
		T _data[1][3];
	};
};


template<class T> struct Num_Data<4,1,T> {
	union {
		struct { T x,y,z,w; };
		T _data[1][4];
	};
};

template<Int COL, Int ROW, class T> 
struct Num_ : public Num_Data<COL, ROW, T> {
	using Data = Num_Data<COL, ROW, T>;
	using Data::_data;
	
	using ElementType = T;
	static constexpr Int kColCount = COL;
	static constexpr Int kRowCount = ROW;
	static constexpr Int kElementCount = COL * ROW;
	
	using MSpan      =      MutSpan<T>;
	using CSpan      =         Span<T>;
	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	AX_INLINE constexpr Num_() = default;

	AX_INLINE constexpr Num_(const T& e0)											{ set(e0); }
	AX_INLINE constexpr Num_(const T& e0, const T& e1)								{ set(e0,e1); }
	AX_INLINE constexpr Num_(const T& e0, const T& e1, const T& e2)					{ set(e0,e1,e2); }
	AX_INLINE constexpr Num_(const T& e0, const T& e1, const T& e2, const T& e3)	{ set(e0,e1,e2,e3); }

	AX_INLINE constexpr Num_(
		const T& e00, const T& e01, const T& e02, const T& e03,
		const T& e10, const T& e11, const T& e12, const T& e13,
		const T& e20, const T& e21, const T& e22, const T& e23,
		const T& e30, const T& e31, const T& e32, const T& e33)
	{
		set(e00,e01,e02,e03,
			e10,e11,e12,e13,
			e20,e21,e22,e23,
			e30,e31,e32,e33);
	}
	
	AX_INLINE constexpr void set(const T& e0) {
		static_assert(kElementCount == 1);
		_data[0][0] = e0;
	}
	
	AX_INLINE constexpr void set(const T& e0, const T& e1) {
		static_assert(kElementCount == 2);
		_data[0][0] = e0;
		_data[0][1] = e1;
	}
	
	AX_INLINE constexpr void set(const T& e0, const T& e1, const T& e2) {
		static_assert(kElementCount == 3);
		_data[0][0] = e0;
		_data[0][1] = e1;
		_data[0][2] = e2;
	}
	
	AX_INLINE constexpr void set(const T& e0, const T& e1, const T& e2, const T& e3) {
		static_assert(kElementCount == 4);
		_data[0][0] = e0;
		_data[0][1] = e1;
		_data[0][2] = e2;
		_data[0][3] = e3;
	}

	AX_INLINE constexpr void set(
		const T& e00, const T& e01, const T& e02, const T& e03,
		const T& e10, const T& e11, const T& e12, const T& e13,
		const T& e20, const T& e21, const T& e22, const T& e23,
		const T& e30, const T& e31, const T& e32, const T& e33) 
	{
		static_assert(kElementCount == 16);
		_data[0][0] = e00;	_data[0][1] = e01;	_data[0][2] = e02;	_data[0][3] = e03;
		_data[1][0] = e10;	_data[1][1] = e11;	_data[1][2] = e12;	_data[1][3] = e13;
		_data[2][0] = e20;	_data[2][1] = e21;	_data[2][2] = e22;	_data[2][3] = e23;
		_data[3][0] = e30;	_data[3][1] = e31;	_data[3][2] = e32;	_data[3][3] = e33;
	}
	
	template<class R>
	void setByCast(const Num_<COL, ROW, R> & src) {
		for (Int r = 0; r < ROW; ++r) {
			for (Int c = 0; c < COL; ++c) {
				_data[r][c] = static_Cast<T>(src._data[r][c]);
			}
		}
	}
	
	AX_NODISCARD AX_INLINE constexpr       T* data()		{ return _data; }
	AX_NODISCARD AX_INLINE constexpr const T* data() const	{ return _data; }
	
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(&_data[0][0]); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(&_data[0][0]); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }

	AX_INLINE constexpr bool inBound(Int col, Int row = 0) const { return row >= 0 && row < ROW && col >= 0 && col < COL; }
	
	AX_INLINE constexpr			T& at			(Int col, Int row = 0)			{ if (!inBound(col, row)) throw Error_IndexOutOfRange(); return at_noBoundCheck(col,row); }
	AX_INLINE constexpr const	T& at			(Int col, Int row = 0) const	{ if (!inBound(col, row)) throw Error_IndexOutOfRange(); return at_noBoundCheck(col,row); }

	AX_INLINE constexpr			T& at_noBoundCheck	(Int col, Int row = 0)			{ return _data[row][col]; }
	AX_INLINE constexpr const	T& at_noBoundCheck	(Int col, Int row = 0) const	{ return _data[row][col]; }
	
	AX_INLINE constexpr bool operator==(const Num_& other) const { return fixedSpan() == other.fixedSpan(); }
};

//----------
template<class T> using Num1_ = Num_<1,1,T>;
template<class T> using Num2_ = Num_<2,1,T>;
template<class T> using Num3_ = Num_<3,1,T>;
template<class T> using Num4_ = Num_<4,1,T>;

template<class T> using Num1x1_ = Num_<1,1,T>;
template<class T> using Num2x2_ = Num_<2,2,T>;
template<class T> using Num3x3_ = Num_<3,3,T>;
template<class T> using Num4x4_ = Num_<4,4,T>;

using Num1h = Num1_<f16>;
using Num2h = Num2_<f16>;
using Num3h = Num3_<f16>;
using Num4h = Num4_<f16>;

using Num1f = Num1_<f32>;
using Num2f = Num2_<f32>;
using Num3f = Num3_<f32>;
using Num4f = Num4_<f32>;

using Num1d = Num1_<f64>;
using Num2d = Num2_<f64>;
using Num3d = Num3_<f64>;
using Num4d = Num4_<f64>;

using Num1i = Num1_<Int>;
using Num2i = Num2_<Int>;
using Num3i = Num3_<Int>;
using Num4i = Num4_<Int>;

using Intx1	= Num1_<Int>;
using Intx2	= Num2_<Int>;
using Intx3	= Num3_<Int>;
using Intx4	= Num4_<Int>;

using i8x1	= Num1_<i8>;
using i8x2	= Num2_<i8>;
using i8x3	= Num3_<i8>;
using i8x4	= Num4_<i8>;

using i16x1	= Num1_<i16>;
using i16x2	= Num2_<i16>;
using i16x3	= Num3_<i16>;
using i16x4	= Num4_<i16>;

using i32x1	= Num1_<i32>;
using i32x2	= Num2_<i32>;
using i32x3	= Num3_<i32>;
using i32x4	= Num4_<i32>;

using i64x1	= Num1_<i64>;
using i64x2	= Num2_<i64>;
using i64x3	= Num3_<i64>;
using i64x4	= Num4_<i64>;

using u8x1	= Num1_<u8>;
using u8x2	= Num2_<u8>;
using u8x3	= Num3_<u8>;
using u8x4	= Num4_<u8>;

using u16x1	= Num1_<u16>;
using u16x2	= Num2_<u16>;
using u16x3	= Num3_<u16>;
using u16x4	= Num4_<u16>;

using u32x1	= Num1_<u32>;
using u32x2	= Num2_<u32>;
using u32x3	= Num3_<u32>;
using u32x4	= Num4_<u32>;

using u64x1	= Num1_<u64>;
using u64x2	= Num2_<u64>;
using u64x3	= Num3_<u64>;
using u64x4	= Num4_<u64>;

using f16x1	= Num1_<f16>;
using f16x2	= Num2_<f16>;
using f16x3	= Num3_<f16>;
using f16x4	= Num4_<f16>;

using f32x1	= Num1_<f32>;
using f32x2	= Num2_<f32>;
using f32x3	= Num3_<f32>;
using f32x4	= Num4_<f32>;

using f64x1	= Num1_<f64>;
using f64x2	= Num2_<f64>;
using f64x3	= Num3_<f64>;
using f64x4	= Num4_<f64>;

using f16x4x4	= Num_<4,4, f16>;
using f32x4x4	= Num_<4,4, f32>;
using f64x4x4	= Num_<4,4, f64>;

using UNorm8x1	= Num1_<UNorm8>;
using UNorm8x2	= Num2_<UNorm8>;
using UNorm8x3	= Num3_<UNorm8>;
using UNorm8x4	= Num4_<UNorm8>;

using SNorm8x1	= Num1_<SNorm8>;
using SNorm8x2	= Num2_<SNorm8>;
using SNorm8x3	= Num3_<SNorm8>;
using SNorm8x4	= Num4_<SNorm8>;

using UNorm16x1	= Num1_<UNorm16>;
using UNorm16x2	= Num2_<UNorm16>;
using UNorm16x3	= Num3_<UNorm16>;
using UNorm16x4	= Num4_<UNorm16>;

using SNorm16x1	= Num1_<SNorm16>;
using SNorm16x2	= Num2_<SNorm16>;
using SNorm16x3	= Num3_<SNorm16>;
using SNorm16x4	= Num4_<SNorm16>;

using UNorm32x1	= Num1_<UNorm32>;
using UNorm32x2	= Num2_<UNorm32>;
using UNorm32x3	= Num3_<UNorm32>;
using UNorm32x4	= Num4_<UNorm32>;

using SNorm32x1	= Num1_<SNorm32>;
using SNorm32x2	= Num2_<SNorm32>;
using SNorm32x3	= Num3_<SNorm32>;
using SNorm32x4	= Num4_<SNorm32>;


template <typename OBJ>
concept CON_IsNum = requires (const OBJ& obj) {
	[]<Int M, Int N, class T>(const Num_<M,N,T>&){}(obj); 
};

} // namespace
