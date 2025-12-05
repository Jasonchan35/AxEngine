module;
#include "AxCore-pch.h"

export module AxCore.String;

export import AxCore.Format;
export import AxCore.UtfUtil;
export import AxCore.Array;

import AxCore.Allocator;
import AxCore.MemoryUtil;
import AxCore.Array;

export namespace ax {

template<class T> inline constexpr Int String_DefaultBufSize = 0;

template<class T, Int BUF_SIZE = String_DefaultBufSize<T>>
class String_;
using String   = String_<Char  >;
using StringA  = String_<CharA >;
using StringW  = String_<CharW >;
using String8  = String_<Char8 >;
using String16 = String_<Char16>;
using String32 = String_<Char32>;

template<Int N> using String_N   = String_<Char  , N>;
template<Int N> using StringA_N  = String_<CharA , N>;
template<Int N> using StringW_N  = String_<CharW , N>;
template<Int N> using String8_N  = String_<Char8 , N>;
template<Int N> using String16_N = String_<Char16, N>;
template<Int N> using String32_N = String_<Char32, N>;

template<class T>
using TempString_  = String_<T, 220>;
using TempString   = TempString_<Char>;
using TempStringA  = TempString_<CharA>;
using TempStringW  = TempString_<CharW>;
using TempString8 = TempString_<Char8>;
using TempString16 = TempString_<Char16>;
using TempString32 = TempString_<Char32>;

template<class T, Int BUF_SIZE> 
class String_ : public IString_<T>, Array_InlineBuffer<T, BUF_SIZE + 1> // +1 for null terminator
{
	using This = String_;
	using Base = IString_<T>;
	using BaseInlineBuffer = Array_InlineBuffer<T, BUF_SIZE + 1>;
	using BaseInlineBuffer::inlineBufPtr;
public:
	using MView = MutStrView_<T>;
	using CView =    StrView_<T>;
	
	AX_INLINE constexpr String_() : Base(inlineBufPtr(), BUF_SIZE) {}
	AX_INLINE constexpr String_(CView view) : String_() { Base::append(view); }
	AX_INLINE constexpr String_(String_ && rhs) : String_() { Base::operator=(std::move(rhs.asIString())); }
	AX_INLINE constexpr String_(const String_ & rhs) : String_(rhs.view()) {}

	template<CON_StrView_<T>... ARGS>
	AX_INLINE constexpr String_(const ARGS&... args) : String_() { Base::append(args...); }

	template<Int N>
	AX_INLINE constexpr String_(String_<T,N> && rhs) : String_() { Base::operator=(std::move(rhs.asIString())); }
	
	template<Int N>
	AX_INLINE constexpr String_(const T (&sz)[N]) : String_() { Base::append(CView(sz)); } 

	constexpr       IString_<T>& asIString()		{ return *this; }
	constexpr const IString_<T>& asIString() const	{ return *this; }
	
	constexpr virtual	~String_() override { Base::clearAndFree(); }

	AX_INLINE constexpr void operator=(StrLit_<T> rhs) { Base::operator=(rhs); }
	AX_INLINE constexpr void operator=(MView      rhs) { Base::operator=(rhs); }
	AX_INLINE constexpr void operator=(CView      rhs) { Base::operator=(rhs); }
	AX_INLINE constexpr void operator=(const IString_<T> & rhs) { Base::operator=(rhs); }

	template<Int N>
	AX_INLINE constexpr void operator=(const T (&sz)[N]) { Base::operator=(CView(sz)); }
	
	// template<Int M>
	// AX_INLINE constexpr void operator=(const String_<T,M> & rhs) { Base::operator=(rhs.view()); }
	
	AX_INLINE constexpr void operator=(IString_<T> && rhs) { Base::move(AX_FORWARD(rhs)); }

	template<Int M>
	AX_INLINE constexpr void operator=(String_<T,M> && rhs) { Base::move(AX_FORWARD(rhs)); }
	
	static constexpr This s_utf(StrViewA  v) { This s; UtfUtil::append(s, v); return s; }
	static constexpr This s_utf(StrViewW  v) { This s; UtfUtil::append(s, v); return s; }
	static constexpr This s_utf(StrView8  v) { This s; UtfUtil::append(s, v); return s; }
	static constexpr This s_utf(StrView16 v) { This s; UtfUtil::append(s, v); return s; }
	static constexpr This s_utf(StrView32 v) { This s; UtfUtil::append(s, v); return s; }

	template<class... ARGS>
	static This s_format(const FormatString_<T, ARGS...> & fmt, const ARGS&... args) {
		This s; s.appendFormat(fmt, AX_FORWARD(args)...); return s;
	}

protected:
	constexpr virtual MemAllocResult<T>	onStorageLocalBuf() override { return MemAllocResult<T>(nullptr, inlineBufPtr(), BUF_SIZE); }
	constexpr virtual MemAllocResult<T>	onStorageMalloc(Int reqSize) override;
	constexpr virtual void				onStorageFree	(T* p) override;
};

template<class T> constexpr bool Type_IsString_ = false; 
template<class T, Int N> constexpr bool Type_IsString_<String_<T,N>> = true; 

template <class T, Int BUF_SIZE> inline
constexpr MemAllocResult<T> String_<T, BUF_SIZE>::onStorageMalloc(Int reqSize) {
	Int newCapacity = reqSize + 1; // +1 for null terminator
	auto* allocator = ax_default_allocator();
	auto buf = allocator->allocArray<T>(newCapacity);
	return MemAllocResult<T>(buf.allocator(), buf.takeOwnership(), buf.size() - 1); // -1 for null terminator
}

template <class T, Int BUF_SIZE> inline
constexpr void String_<T, BUF_SIZE>::onStorageFree(T* p) {
	auto* d = inlineBufPtr(); 
	if (p == d) return;
	auto* allocator = ax_default_allocator();
	allocator->dealloc(p);
}

} // namespace
