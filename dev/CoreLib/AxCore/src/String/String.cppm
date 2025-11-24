module;
#include "AxCore-pch.h"

export module AxCore.String;

export import AxCore.Format;
export import AxCore.UtfUtil;
export import AxCore.Array;

import AxCore.Allocator;
import AxCore.MemoryUtil;
import AxCore.InlineBuffer;

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


template< class OUT_CH, class FMT_CH, class ... ARGS> 
AX_INLINE void ax_format_to_internal(IString_<OUT_CH> & output, const FormatString_<FMT_CH, ARGS...> & fmt, ARGS&&... args) {
	// std::format only support char and wchar_t format string
	using OUT_FMT_CH = std::conditional_t<std::is_same_v<FMT_CH, CharA>, CharA, CharW>; 
	FormatArgs_<OUT_FMT_CH> format_args = std::make_format_args<FormatContext_<OUT_FMT_CH>>(args...);
	
	StrView_<OUT_FMT_CH> out_fmt;
	TempString_<OUT_FMT_CH> tmp_str;
	if constexpr (std::is_same_v<OUT_FMT_CH, FMT_CH>) {
		out_fmt = fmt.get();
	} else {
		tmp_str.appendUtf(fmt.get());
		out_fmt = tmp_str;
	}
	
	try {
		output.reserve(output.size() + fmt.size() + format_args._Estimate_required_capacity());
		std::vformat_to(IStringBackInserter_<OUT_FMT_CH, OUT_CH>(output), out_fmt.to_string_view(), format_args);
			
	} catch (std::exception& e) {
		auto msg = TempString::s_format("Exception in format={}, {})", out_fmt, e.what());
		__ax_internal_logError(msg.c_str());
		throw Error_Format();
	}
}

template<class CH, class... ARGS> AX_INLINE
void FmtTo(IString_<CH> & output, const FormatString_<CharA, ARGS...> & fmt, ARGS&&... args) {
	return ax_format_to_internal<CH, CharA, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class T, class... ARGS> AX_INLINE
void FmtTo(IString_<T> & output, const FormatString_<CharW, ARGS...> & fmt, ARGS&&... args) {
	return ax_format_to_internal<T, CharW, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class T, class... ARGS> AX_INLINE
void FmtTo(IString_<T> & output, const FormatString_<Char8, ARGS...> & fmt, ARGS&&... args) {
	return ax_format_to_internal<T, Char8, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class T, class... ARGS> AX_INLINE
void FmtTo(IString_<T> & output, const FormatString_<Char16, ARGS...> & fmt, ARGS&&... args) {
	return ax_format_to_internal<T, Char16, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class T, class... ARGS> AX_INLINE
void FmtTo(IString_<T> & output, const FormatString_<Char32, ARGS...> & fmt, ARGS&&... args) {
	return ax_format_to_internal<T, Char32, ARGS...>(output, fmt, AX_FORWARD(args)...);
}



template<class... ARGS> AX_INLINE
StringA Fmt(FormatString_<CharA, ARGS...> && fmt, ARGS&&... args) { StringA str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }

template<class... ARGS> AX_INLINE
StringW Fmt(FormatString_<CharW, ARGS...> && fmt, ARGS&&... args) { StringW str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }

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

template<class FMT_CH>
struct UtfFormatter_ : public FormatterBase_<FMT_CH> {
	using Base = FormatterBase_<FMT_CH>;
	
protected:
	template<class CH>
	constexpr auto utfFormat(const ax::MutStrView_<CH>& obj, ax::FormatContext_<FMT_CH>& ctx) const {
		if constexpr (std::is_same_v<CH, FMT_CH>) {
			return Base::format(obj, ctx);
		} else {
			auto tmp = ax::TempString_<FMT_CH>::s_utf(obj);
			return Base::format(tmp, ctx);
		}
	}
};

} // namespace

//----- global namespace ----------

// Wrapper to CustomClass::onFormat()
template<class OBJ, class FMT_CH> requires ax::Format_HasOnFormat_<OBJ, FMT_CH>
struct std::formatter<OBJ, FMT_CH> : public ax::UtfFormatter_<FMT_CH> {
	using Base = ax::UtfFormatter_<FMT_CH>;

	constexpr auto parse(ax::FormatParseContext_<FMT_CH>& ctx) {
		if constexpr (ax::Format_HasOnParse_<OBJ>) {
			OBJ::onFormatParse(ctx);
			return ctx.end();
		} else {
			return Base::parse(ctx);
		}
	}

	auto format(const OBJ& obj, ax::FormatContext_<FMT_CH>& ctx) const {
		ax::Format_<FMT_CH> format(*this, ctx);
		obj.onFormat(format);
		return ctx.out();
	}
};

template <class OBJ_CH, class FMT_CH>
struct std::formatter<ax::MutStrView_<OBJ_CH>, FMT_CH> : public ax::UtfFormatter_<FMT_CH> {
	constexpr auto format(const ax::MutStrView_<OBJ_CH>& obj, ax::FormatContext_<FMT_CH>& ctx) const {
		return ax::UtfFormatter_<FMT_CH>::utfFormat(obj, ctx);
	}
};

template <class OBJ_CH, class FMT_CH>
struct std::formatter<ax::MutStrLit_<OBJ_CH>, FMT_CH> : public ax::UtfFormatter_<FMT_CH> {
	constexpr auto format(const ax::MutStrLit_<OBJ_CH>& obj, ax::FormatContext_<FMT_CH>& ctx) const {
		return ax::UtfFormatter_<FMT_CH>::utfFormat(ax::StrView_<OBJ_CH>(obj), ctx);
	}
};

template <class OBJ_CH, ax::Int N, class FMT_CH>
struct std::formatter<ax::String_<OBJ_CH, N>, FMT_CH> : public ax::UtfFormatter_<FMT_CH> {
	using Base = ax::FormatterBase_<FMT_CH>;
	auto format(const ax::String_<OBJ_CH, N>& obj, ax::FormatContext_<FMT_CH>& ctx) const {
		return ax::UtfFormatter_<FMT_CH>::utfFormat(ax::StrView_<OBJ_CH>(obj), ctx);
	}
};

template <class OBJ_CH, class FMT_CH> requires (!std::is_same_v<OBJ_CH, FMT_CH>)
struct std::formatter<std::basic_string_view<OBJ_CH>, FMT_CH> : public ax::UtfFormatter_<FMT_CH> {
	constexpr auto format(const std::basic_string_view<OBJ_CH> obj, ax::FormatContext_<FMT_CH>& ctx) const {
		return ax::UtfFormatter_<FMT_CH>::utfFormat(ax::StrView_<OBJ_CH>(obj), ctx);
	}
};

template <class OBJ_CH, size_t N, class FMT_CH> requires (!std::is_same_v<OBJ_CH, FMT_CH>)
struct std::formatter<OBJ_CH[N], FMT_CH> : public ax::UtfFormatter_<FMT_CH> {
	constexpr auto format(const OBJ_CH (&obj)[N], ax::FormatContext_<FMT_CH>& ctx) const {
		return ax::UtfFormatter_<FMT_CH>::utfFormat(ax::StrView_<OBJ_CH>(obj), ctx);
	}
};
