module;


export module AxCore.BinIO_Reader;
export import AxCore.Logger;

export namespace ax {

template<class T> struct BinIO_Handler;

template<class T>
struct BinIO_Handler {
	template<class SE> void onIO(SE& se, T& value) {
		static_assert(!Type_IsSpan<T>,
			"please use FixedSpan / IArray for IO, since Span cannot do dynamic resize");
		static_assert(!Type_IsStrView<T>, 
			"please use IString for IO, since StrView cannot do dynamic resize");

		value.onBinaryIO(se);
	}
};

class BinIO_Reader : public NonCopyable {
public:
	BinIO_Reader(ByteSpan data) 
		: _data(data.data())
		, _cur(data.data())
		, _end(data.end())
	{}

	AX_INLINE void io(i8 & value)	{ io_vary(value); }
	AX_INLINE void io(i16& value)	{ io_vary(value); }
	AX_INLINE void io(i32& value)	{ io_vary(value); }
	AX_INLINE void io(i64& value)	{ io_vary(value); }

	AX_INLINE void io(u8 & value)	{ io_vary(value); }
	AX_INLINE void io(u16& value)	{ io_vary(value); }
	AX_INLINE void io(u32& value)	{ io_vary(value); }
	AX_INLINE void io(u64& value)	{ io_vary(value); }

	AX_INLINE void io(f32& value)	{ io_fixed(value); }
	AX_INLINE void io(f64& value)	{ io_fixed(value); }

	AX_INLINE void io(char& value) { io_vary(value); }

	template<class T> AX_INLINE
	void io(T& value) { BinIO_Handler(*this, value); }

	template<class T, Int N> inline void io_fixed_span(MutFixedSpan<T, N> obj);
//	template<class T> inline void io_span(MutSpan<T> value);
	template<class T> inline void io_array(IArray<T>& obj);
	template<class T> inline void io_string(IString_<T>& obj);

	template<class T> AX_INLINE
	BinIO_Reader& operator<<(T& value) { io(value); return *this; }

//----------
	AX_INLINE void io_fixed(i8 & value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed(i16& value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed(i32& value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed(i64& value)	{ _io_fixed_le(value); }

	AX_INLINE void io_fixed(u8 & value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed(u16& value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed(u32& value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed(u64& value)	{ _io_fixed_le(value); }

	AX_INLINE void io_fixed(f32& value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed(f64& value)	{ _io_fixed_le(value); }
//-----
	AX_INLINE void io_fixed_le(i8 & value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed_le(i16& value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed_le(i32& value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed_le(i64& value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed_le(u8 & value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed_le(u16& value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed_le(u32& value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed_le(u64& value)	{ _io_fixed_le(value); }

	AX_INLINE void io_fixed_le(f32& value)	{ _io_fixed_le(value); }
	AX_INLINE void io_fixed_le(f64& value)	{ _io_fixed_le(value); }
//-----
	AX_INLINE void io_fixed_be(i8 & value)	{ _io_fixed_be(value); }
	AX_INLINE void io_fixed_be(i16& value)	{ _io_fixed_be(value); }
	AX_INLINE void io_fixed_be(i32& value)	{ _io_fixed_be(value); }
	AX_INLINE void io_fixed_be(i64& value)	{ _io_fixed_be(value); }

	AX_INLINE void io_fixed_be(u8 & value)	{ _io_fixed_be(value); }
	AX_INLINE void io_fixed_be(u16& value)	{ _io_fixed_be(value); }
	AX_INLINE void io_fixed_be(u32& value)	{ _io_fixed_be(value); }
	AX_INLINE void io_fixed_be(u64& value)	{ _io_fixed_be(value); }

	AX_INLINE void io_fixed_be(f32& value)	{ _io_fixed_be(value); }
	AX_INLINE void io_fixed_be(f64& value)	{ _io_fixed_be(value); }
//-----

	AX_INLINE void io_vary(i8 & value)	{ _io_vary_signed<u8 >(value); }
	AX_INLINE void io_vary(i16& value)	{ _io_vary_signed<u16>(value); }
	AX_INLINE void io_vary(i32& value)	{ _io_vary_signed<u32>(value); }
	AX_INLINE void io_vary(i64& value)	{ _io_vary_signed<u64>(value); }

	AX_INLINE void io_vary(u8 & value)	{ _io_vary_unsigned(value); }
	AX_INLINE void io_vary(u16& value)	{ _io_vary_unsigned(value); }
	AX_INLINE void io_vary(u32& value)	{ _io_vary_unsigned(value); }
	AX_INLINE void io_vary(u64& value)	{ _io_vary_unsigned(value); }

	AX_INLINE void io_vary(char& value)	{ _io_vary_unsigned(reinterpret_cast<u8&>(value)); }

	AX_INLINE const u8* cur() const { return _cur; }
	AX_INLINE Int		remain() const { return _end - _cur; }
	AX_INLINE const u8* advance(Int n);

	void io_raw(MutByteSpan data) {
		AX_GCC_WARNING_PUSH_AND_DISABLE("-Wunsafe-buffer-usage")
		auto* src = advance(data.size());
		std::copy(src, src + data.size(), data.data());
		AX_GCC_WARNING_POP()
	}

private:
	template<class T> AX_INLINE void _io_fixed_le(T& obj);
	template<class T> AX_INLINE void _io_fixed_be(T& obj);
	template<class T> AX_INLINE void _io_vary_unsigned(T& obj);
	template<class U, class T> AX_INLINE void _io_vary_signed(T& obj);

	const u8* _data = nullptr;
	const u8* _cur  = nullptr;
	const u8* _end  = nullptr;
};

AX_INLINE const u8* BinIO_Reader::advance(Int n) {
	AX_GCC_WARNING_PUSH_AND_DISABLE("-Wunsafe-buffer-usage")

	if (remain() < n) {
		throw Error_Undefined();
	}
	const u8* old = _cur;
	_cur += n;
	return old;

	AX_GCC_WARNING_POP()

}

template<class T, Int N> inline
void BinIO_Reader::io_fixed_span(MutFixedSpan<T, N> obj) {
	for (Int i = 0; i < N; i++) {
		io(obj[i]);
	}
}

//template<class T> inline
//void BinIO_Reader::io_span(MutSpan<T> value) {
//	Int len = 0;
//	io(len);
//	if (value.size() != len)
//		throw Error_Undefined();
//
//	for (Int i = 0; i < len; i++) {
//		io(v[i]);
//	}
//}

template<class T> inline
void BinIO_Reader::io_array(IArray<T>& obj) {
	Int len = 0;
	io(len);
	obj.resize(len);
	try {
		for (Int i = 0; i < len; i++) {
			io(obj[i]);
		}
	} catch(...) {
		obj.clear();
		throw;
	}
}

template<class CH> inline
void BinIO_Reader::io_string(IString_<CH>& obj) {
	if constexpr (sizeof(CH) == 1) {
		Int len = 0;
		io(len);
		obj.resize(len);
		try {
			io_raw(obj.toByteSpan());
		} catch(...) {
			obj.clear();
			throw;
		}
	} else {
		TempStringA tmp;
		io_string(tmp);
		obj.setUtf(tmp);
	}
}

template<class T> AX_INLINE
void BinIO_Reader::_io_fixed_le(T& obj) {
	auto* p = advance(sizeof(obj));
	obj = ByteOrder::LittleEndian::toHost(*reinterpret_cast<const T*>(p));
}

template<class T> AX_INLINE
void BinIO_Reader::_io_fixed_be(T& obj) {
	auto* p = advance(sizeof(obj));
	obj = ByteOrder::BigEndian::toHost(*reinterpret_cast<const T*>(p));
}

template<class T> AX_INLINE
void BinIO_Reader::_io_vary_unsigned(T& obj) {
	Int bit = 0;
	obj = 0;
	for(Int i = 0; i < AX_SIZEOF(T) + 1; i++) {
		u8 t = *advance(1);
		obj |= static_cast<T>( t & 0x7F ) << bit;
		if((t & 0x80) == 0) return;
		bit += 7;
		if( bit > AX_SIZEOF(T) * 8 ) {
			throw Error_Undefined();
		}
	}
}

template<class U, class T> AX_INLINE
void BinIO_Reader::_io_vary_signed(T& obj) {
// ZigZag encoding - https://developers.google.com/protocol-buffers/docs/encoding
	static_assert(sizeof(U) == sizeof(T));
	U tmp;
	_io_vary_unsigned(tmp);
	obj = static_cast<T>(tmp >> 1) ^ -static_cast<T>(tmp & 1);
}

} // namespace