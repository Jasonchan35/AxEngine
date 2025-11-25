module;
#include "AxPlatform-pch.h"

export module AxPlatform.Formatter;

export import AxPlatform.String;

export namespace ax {

template<class OBJ>
concept CON_onFormatParse_ = requires(const OBJ& obj) {
	{ OBJ::onFormatParse };
};

template<class OBJ, class FMT_CH>
concept CON_onFormat_ = requires(const OBJ& obj, Format_<FMT_CH> & fmt) {
	//	{ obj.onFormat(fmt) } -> std::same_as<void>;
	true;
};


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


template< class STR_CH, class IN_FMT_CH, class ... ARGS> 
AX_INLINE void ax_format_to_internal(IString_<STR_CH> & output, const FormatString_<IN_FMT_CH, ARGS...> & fmt, ARGS&&... args) {
	// std::format only support char and wchar_t format string
	using FMT_CH = std::conditional_t<std::is_same_v<IN_FMT_CH, CharA>, CharA, CharW>; 
	FormatArgs_<FMT_CH> format_args = std::make_format_args<FormatContext_<FMT_CH>>(args...);
	
	StrView_<FMT_CH> out_fmt;
	TempString_<FMT_CH> tmp_str;
	if constexpr (std::is_same_v<FMT_CH, IN_FMT_CH>) {
		out_fmt = fmt.get();
	} else {
		tmp_str.appendUtf(fmt.get());
		out_fmt = tmp_str;
	}
	
	try {
		output.reserve(output.size() + fmt.size() + format_args._Estimate_required_capacity());
		std::vformat_to(IStringBackInserter_<STR_CH>(output), out_fmt.to_string_view(), format_args);
			
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

template<class... ARGS> AX_INLINE
String8 Fmt(FormatString_<Char8, ARGS...> && fmt, ARGS&&... args) { String8 str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }

template<class... ARGS> AX_INLINE
String16 Fmt(FormatString_<Char16, ARGS...> && fmt, ARGS&&... args) { String16 str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }

template<class... ARGS> AX_INLINE
String32 Fmt(FormatString_<Char32, ARGS...> && fmt, ARGS&&... args) { String32 str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }


} // namespace


//----- global namespace ----------

// Wrapper to CustomClass::onFormat()
template<class OBJ, class FMT_CH> requires ax::CON_onFormat_<OBJ, FMT_CH>
struct std::formatter<OBJ, FMT_CH> : public ax::UtfFormatter_<FMT_CH> {
	using Base = ax::UtfFormatter_<FMT_CH>;

	constexpr auto parse(ax::FormatParseContext_<FMT_CH>& ctx) {
		if constexpr (ax::CON_onFormatParse_<OBJ>) {
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
