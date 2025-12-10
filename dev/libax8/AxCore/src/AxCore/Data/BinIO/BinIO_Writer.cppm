module;
#include "AxCore-pch.h"

export module AxCore.BinIO_Writer;
export import AxCore.BinIO_Reader;

export namespace ax {


class BinIO_Writer : public NonCopyable {
public:
	BinIO_Writer(IArray<u8>& buf) : _buf(&buf) {}

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

	template<class T, Int N> void io_fixed_span(FixedSpan<T, N> value);
	template<class T> void io_array(IArray<T>& obj);
	template<class T> void io_string(IString_<T>& value);

	template<class T> AX_INLINE
	BinIO_Writer& operator<<(T& value) { io(value); return *this; }

//-----------
	AX_INLINE void io_fixed(i8 & value)	{ _io_fixed(value); }
	AX_INLINE void io_fixed(i16& value)	{ _io_fixed(value); }
	AX_INLINE void io_fixed(i32& value)	{ _io_fixed(value); }
	AX_INLINE void io_fixed(i64& value)	{ _io_fixed(value); }

	AX_INLINE void io_fixed(u8 & value)	{ _io_fixed(value); }
	AX_INLINE void io_fixed(u16& value)	{ _io_fixed(value); }
	AX_INLINE void io_fixed(u32& value)	{ _io_fixed(value); }
	AX_INLINE void io_fixed(u64& value)	{ _io_fixed(value); }

	AX_INLINE void io_fixed(f32& value)	{ _io_fixed(value); }
	AX_INLINE void io_fixed(f64& value)	{ _io_fixed(value); }

	AX_INLINE void io_vary(i8 & value)	{ _io_vary_signed<u8 >(value); }
	AX_INLINE void io_vary(i16& value)	{ _io_vary_signed<u16>(value); }
	AX_INLINE void io_vary(i32& value)	{ _io_vary_signed<u32>(value); }
	AX_INLINE void io_vary(i64& value)	{ _io_vary_signed<u64>(value); }

	AX_INLINE void io_vary(u8 & value)	{ _io_vary_unsigned(value); }
	AX_INLINE void io_vary(u16& value)	{ _io_vary_unsigned(value); }
	AX_INLINE void io_vary(u32& value)	{ _io_vary_unsigned(value); }
	AX_INLINE void io_vary(u64& value)	{ _io_vary_unsigned(value); }

	AX_INLINE void io_vary(char& value)	{ _io_vary_unsigned(reinterpret_cast<u8&>(value)); }

	AX_INLINE u8* advance(Int n);

	void io_raw(ByteSpan data) { _buf->appendRange(data); }

private:
	template<class T> AX_INLINE void _io_fixed(T& value);
	template<class T> AX_INLINE void _io_vary_unsigned(T& value);
	template<class U, class T> AX_INLINE void _io_vary_signed(T& value);

private:
	IArray<u8>* _buf = nullptr;
};

AX_INLINE
u8* BinIO_Writer::advance(Int n) {
	AX_GCC_WARNING_PUSH_AND_DISABLE("-Wunsafe-buffer-usage")

	Int oldSize = _buf->size();
	_buf->resize(oldSize + n);
	return _buf->data() + oldSize;

	AX_GCC_WARNING_POP()
}

template<class T, Int N> inline
void BinIO_Writer::io_fixed_span(FixedSpan<T, N> value) {
	for (Int i = 0; i < N; i++) {
		io(value[i]);
	}
}

template<class T> inline
void BinIO_Writer::io_array(IArray<T>& obj) {
	Int len = obj.size();
	io(len);
	for (Int i = 0; i < len; i++) {
		io(obj[i]);
	}
}

template<class CH> inline
void BinIO_Writer::io_string(IString_<CH>& obj) {
	if constexpr (sizeof(CH) == 1) {
		Int len = obj.size();
		io(len);
		io_raw(obj.toByteSpan());
	} else {
		io_string(TempStringA::s_utf(obj));
	}
}

template<class T> AX_INLINE
void BinIO_Writer::_io_fixed(T& value) {
	auto* p = advance(sizeof(value));
	*reinterpret_cast<T*>(p) = ByteOrder::LittleEndian::fromHost(value);
}

template<class T> AX_INLINE
void BinIO_Writer::_io_vary_unsigned(T& value) {
	T tmp = value;
	for(Int i = 0; i < AX_SIZEOF(T) + 1; i++) {
		u8 highBit = tmp >= 0x80 ? 0x80 : 0;
		_buf->append(static_cast<u8>(tmp) | highBit);
		tmp >>= 7;
		if (tmp == 0) return;
	}
	throw Error_Undefined();
}

template<class U, class T> AX_INLINE
void BinIO_Writer::_io_vary_signed(T& value) {
// ZigZag encoding - https://developers.google.com/protocol-buffers/docs/encoding
	static_assert(sizeof(U) == sizeof(T));
	U tmp = static_cast<U>( ( value << 1 ) ^ ( value >> (AX_SIZEOF(T)*8-1) ) );
	_io_vary_unsigned(tmp);
}

} // namespace