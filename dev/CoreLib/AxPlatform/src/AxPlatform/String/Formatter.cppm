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

template <class T, class FMT_CH> requires Type_IsFundamental<T>
class FormatHandler<T, FMT_CH> {
public:
	void onFormat(const T & obj, Format_<FMT_CH> & fmt) {
		std::formatter<T, FMT_CH> tmp;
		tmp.format(obj, fmt.formatContext);
	}
};

template <class T, class FMT_CH> requires Type_IsConvertiableToStrViewT<T>
class FormatHandler<T, FMT_CH> {
public:
	void onFormat(const T & obj, Format_<FMT_CH> & fmt) {
		
		if constexpr (std::is_convertible_v<T, StrView_<FMT_CH>>) {
			auto sv = StrView_<FMT_CH>(obj); 
			fmt.formatter.format(sv.to_string_view(), fmt.formatContext);
		} else {
			auto utf = TempString_<FMT_CH>::s_utf(obj);
			fmt.formatter.format(utf.to_string_view(), fmt.formatContext);
		}
	}
};

template< class STR_CH, class IN_FMT_CH, class ... ARGS> 
inline void ax_format_to_internal(IString_<STR_CH> & output, const FormatString_<IN_FMT_CH, ARGS...> & fmt, const ARGS&... args) {
	// std::format only support char and wchar_t format string
	using FMT_CH = std::conditional_t<std::is_same_v<IN_FMT_CH, CharA>, CharA, CharW>;
	
	StrView_<FMT_CH> out_fmt;
	TempString_<FMT_CH> tmp_str;
	if constexpr (std::is_same_v<FMT_CH, IN_FMT_CH>) {
		out_fmt = fmt.get();
	} else {
		tmp_str.appendUtf(fmt.get());
		out_fmt = tmp_str;
	}

	auto func = [&](const StdFormatArgs_<FMT_CH> & format_args) {
		output.reserve(output.size() + fmt.size() + format_args._Estimate_required_capacity());
		std::vformat_to(IStringBackInserter_<STR_CH>(output), out_fmt.to_string_view(), format_args);
	};
	
	try {
		// std::make_format_args() must be call inside function call parameter to keep lifetime, otherwise the stored arg will be dangling 
		func(std::make_format_args<StdFormatContext_<FMT_CH>>(args...));
	} catch (std::exception& e) {
		auto msg = TempString::s_format("Format: Exception: {}, ArgCount={} format_string=[{}])", e.what(), sizeof...(args), out_fmt);
		__ax_internal_logError(msg.c_str());
		throw Error_Format();
	}
}

template<class CH, class... ARGS> constexpr 
void FmtTo(IString_<CH> & output, const FormatString_<CharA, ARGS...> & fmt, const ARGS&... args) {
	return ax_format_to_internal<CH, CharA, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class T, class... ARGS> constexpr
void FmtTo(IString_<T> & output, const FormatString_<CharW, ARGS...> & fmt, const ARGS&... args) {
	return ax_format_to_internal<T, CharW, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class T, class... ARGS> constexpr
void FmtTo(IString_<T> & output, const FormatString_<Char8, ARGS...> & fmt, const ARGS&... args) {
	return ax_format_to_internal<T, Char8, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class T, class... ARGS> constexpr
void FmtTo(IString_<T> & output, const FormatString_<Char16, ARGS...> & fmt, const ARGS&... args) {
	return ax_format_to_internal<T, Char16, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class T, class... ARGS> constexpr
void FmtTo(IString_<T> & output, const FormatString_<Char32, ARGS...> & fmt, const ARGS&... args) {
	return ax_format_to_internal<T, Char32, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class... ARGS> constexpr
TempStringA Fmt(FormatString_<CharA, ARGS...> && fmt, const ARGS&... args) { TempStringA str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }

template<class... ARGS> constexpr
TempStringW Fmt(FormatString_<CharW, ARGS...> && fmt, const ARGS&... args) { TempStringW str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }

template<class... ARGS> constexpr
TempString8 Fmt(FormatString_<Char8, ARGS...> && fmt, const ARGS&... args) { TempString8 str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }

template<class... ARGS> constexpr
TempString16 Fmt(FormatString_<Char16, ARGS...> && fmt, const ARGS&... args) { TempString16 str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }

template<class... ARGS> constexpr
TempString32 Fmt(FormatString_<Char32, ARGS...> && fmt, const ARGS&... args) { TempString32 str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }

} // namespace


//----- global namespace ----------

// Wrapper to CustomClass::onFormat()
template<class T, class FMT_CH> requires	ax::CON_onFormat_<T, FMT_CH>
struct std::formatter<T, FMT_CH> : public  ax::StdFormatter_<FMT_CH> {
	using Base = ax::StdFormatter_<FMT_CH>;

	constexpr auto parse(ax::StdFormatParseContext_<FMT_CH>& ctx) {
		if constexpr (ax::CON_onFormatParse_<T>) {
			T::onFormatParse(ctx);
			return ctx.end();
		} else {
			return Base::parse(ctx);
		}
	}

	auto format(const T& obj, ax::StdFormatContext_<FMT_CH>& ctx) const {
		ax::Format_<FMT_CH> format(*this, ctx);
		ax::FormatHandler<T, FMT_CH> handler;
		handler.onFormat(obj, format);
		return ctx.out();
	}
};