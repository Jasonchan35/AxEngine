module;
#include "AxPlatform-pch.h"

export module AxPlatform.String;

export import AxPlatform.Format;
export import AxPlatform.UtfUtil;
export import AxPlatform.Array;

import AxPlatform.Allocator;
import AxPlatform.MemoryUtil;
import AxPlatform.InlineBuffer;

export namespace ax {

template<class T> inline
constexpr Int String_DefaultBufSize = []() {
	Int size = 0;
	Int s = ax_sizeof<Int> / ax_sizeof<T>;
	Int padding = s > 0 ? s - 1 : 0;; // -1 for null terminator
	return size + padding;
}();

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
using TempString_  = String_<T, 512>; // long enough to hold file path
using TempString   = TempString_<Char>;
using TempStringA  = TempString_<CharA>;
using TempStringW  = TempString_<CharW>;
using TempString16 = TempString_<Char16>;
using TempString32 = TempString_<Char32>;

template<class T, Int BUF_SIZE> 
class String_ : public IString_<T>, InlineBuffer<T, BUF_SIZE + 1> // +1 for null terminator
{
	using This = String_;
	using Base = IString_<T>;
	using BaseInlineBuffer = InlineBuffer<T, BUF_SIZE + 1>;
	using BaseInlineBuffer::inlineBufPtr;
public:
	using View = StrView_<T>;
	
	AX_INLINE String_() : Base(inlineBufPtr(), BUF_SIZE) {}
	AX_INLINE String_(View view) : String_() { Base::append(view); }
	AX_INLINE String_(String_ && rhs) : String_() { Base::operator=(std::move(rhs.asIString())); }

	template<Int N>
	AX_INLINE String_(const T (&sz)[N]) : String_() { Base::append(StrView_make(sz)); } 

	constexpr       IString_<T>& asIString()		{ return *this; }
	constexpr const IString_<T>& asIString() const	{ return *this; }

	constexpr virtual	~String_() override { Base::clearAndFree(); }

	static This s_utf(StrViewA  v) { This s; UtfUtil::append(s, v); return s; }
	static This s_utf(StrViewW  v) { This s; UtfUtil::append(s, v); return s; }
	static This s_utf(StrView8  v) { This s; UtfUtil::append(s, v); return s; }
	static This s_utf(StrView16 v) { This s; UtfUtil::append(s, v); return s; }
	static This s_utf(StrView32 v) { This s; UtfUtil::append(s, v); return s; }

	template<class... ARGS>
	static This s_format(const FormatString_<Char, ARGS...> & fmt, ARGS&&... args) {
		This s; s.appendFmt(fmt, AX_FORWARD(args)...); return s;
	}

protected:
	constexpr virtual MemAllocResult<T>	onStorageLocalBuf() override { return MemAllocResult<T>(nullptr, inlineBufPtr(), BUF_SIZE); }
	constexpr virtual	MemAllocResult<T>	onStorageMalloc(Int reqSize) override;
	constexpr virtual	void			onStorageFree	(T* p) override;
};

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
