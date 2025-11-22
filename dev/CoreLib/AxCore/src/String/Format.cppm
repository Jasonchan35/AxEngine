export module AxCore.Format;

#include "AxBase.h"

export import AxCore.IString;
import AxCore.Debug;
import <format>;

export namespace ax {

// using FmtChar = char; // wchar_t

template<class OUT_CH>	
struct FmtBackInserter_ {
	using This = FmtBackInserter_;
	using OutIString = IString_<OUT_CH>;
	
	using difference_type = ptrdiff_t;

	constexpr explicit FmtBackInserter_(OutIString& s) noexcept : _s(&s) {}

	constexpr This& operator=(const OUT_CH&  ch) { _s->appendChar(ch); return *this; }
	constexpr This& operator=(      OUT_CH&& ch) { _s->appendChar(std::move(ch)); return *this; }

	constexpr This& operator*() noexcept { return *this; }
	constexpr This& operator++() noexcept { return *this; }
	constexpr This  operator++(int) noexcept { return *this; }

protected:
	OutIString* _s = nullptr;
};	

// using FmtContext = std::format_context;
template<class FMT_CH>
using FmtContext_ = std::_Default_format_context<FMT_CH>;

// using FmtParseContext = std::format_parse_context;
template<class FMT_CH>
using FmtParseContext_ = std::basic_format_parse_context<FMT_CH>;

// template<class... ARGS> using FormatString = std::format_string<ARGS...>;
template<class FMT_CH, class... ARGS>
using FmtFormatString_ = std::basic_format_string<FMT_CH, std::type_identity_t<ARGS>...>;

template<class FMT_CH>
using FmtArgs_ = std::basic_format_args<FmtContext_<FMT_CH>>;

template<class T> AX_INLINE
constexpr void ax_vformat_to(IString_<T> & outStr, const std::basic_string_view<T>& fmt, const FmtArgs_<T>& args) {
	outStr.reserve(fmt.size() + args._Estimate_required_capacity());
	try {
		std::vformat_to(FmtBackInserter_(outStr), fmt, args);
	} catch (std::exception& e) {
		auto msg  = std::format("Exception in format={}, {})", fmt, e.what());
		Debug::_internal_logError(msg.c_str());
		throw Error_Format();
	}
}

template<class... ARGS> AX_INLINE
constexpr void ax_format_to(IStringA & outStr, const std::format_string<ARGS...> & fmt, ARGS&&... args) {
	ax_vformat_to<Char>(outStr, fmt.get(), std::make_format_args<FmtContext_<Char>>(args...));
}

template<class... ARGS> AX_INLINE
constexpr void ax_format_to(IStringW & outStr, const std::wformat_string<ARGS...> & fmt, ARGS&&... args) {
	ax_vformat_to<CharW>(outStr, fmt.get(), std::make_format_args<FmtContext_<CharW>>(args...));
}

#if AX_LANG_CPP_23 // requires c++23 to support char type other than char and wchar_t 

template<class... ARGS> AX_INLINE
constexpr String16 ax_format(FmtFormatString_<Char16, ARGS...> fmt, ARGS&&... args) {
	return ax_format_<Char16>(fmt.get(), AX_FORWARD(args)...);
}

#endif

//--- Formatter
template<class FMT_CH>
struct FmtFormatterBase_ {
	using FmtContext = FmtContext_<FMT_CH>;
	using FmtParseContext = FmtParseContext_<FMT_CH>;
	
	constexpr auto parse(FmtParseContext& ctx) const { return ctx.begin(); }
};

template<class OBJ, class FMT_CH>
concept FmtHasOnFormat_ = requires(const OBJ& obj, FmtContext_<FMT_CH>& ctx) {
	{ obj.OnFormat(ctx) } -> std::same_as<void>;
};

} // namespace

// warp to ax::FmtFormatter;
template<class OBJ, class FMT_CH> requires ax::FmtHasOnFormat_<OBJ, FMT_CH>
struct std::formatter<OBJ, FMT_CH> : public ax::FmtFormatterBase_<FMT_CH> {
	constexpr auto format(const OBJ& v, ax::FmtContext_<FMT_CH>& ctx) const {
		v.onFormat(ctx);
		return ctx.out();
	}
};

template <class CH, class FMT_CH>
struct std::formatter<ax::MutStrView_<CH>, FMT_CH> : public ax::FmtFormatterBase_<FMT_CH> {
	using Base = ax::FmtFormatterBase_<FMT_CH>;
	using FmtContext = ax::FmtContext_<FMT_CH>;

	template<class Context>
	constexpr auto format(const ax::MutStrView_<CH> & obj, Context& ctx) const {
		return std::format_to(ctx.out(), "2");
	}
};

template <class FMT_CH>
struct std::formatter<std::wstring_view, FMT_CH> : public ax::FmtFormatterBase_<FMT_CH> {
	using Base = ax::FmtFormatterBase_<FMT_CH>;
	using FmtContext = ax::FmtContext_<FMT_CH>;
	
	template <class FormatContext>
	constexpr auto format(const std::wstring_view& obj, FormatContext& ctx) const {
		return std::format_to(ctx.out(), "1");
	}
};

template <class FMT_CH>
struct std::formatter<wchar_t, FMT_CH> : public ax::FmtFormatterBase_<FMT_CH> {
	using Base = ax::FmtFormatterBase_<FMT_CH>;
	using FmtContext = ax::FmtContext_<FMT_CH>;

	template <class FormatContext>
	constexpr auto format(const wchar_t &obj, FormatContext& ctx) const {
		return std::format_to(ctx.out(), "1");
	}
};

template <class FMT_CH>
struct std::formatter<char16_t, FMT_CH> : public ax::FmtFormatterBase_<FMT_CH> {
	using Base = ax::FmtFormatterBase_<FMT_CH>;
	using FmtContext = ax::FmtContext_<FMT_CH>;
	
	template <class FormatContext>
	constexpr auto format(const char16_t &obj, FormatContext& ctx) const {
		return std::format_to(ctx.out(), "1");
	}
};

//std lib doesn't support char[N] for same FMT_CH
template <ax::CharType CH, size_t N, class FMT_CH> requires !std::is_same_v<char, FMT_CH>
struct std::formatter<CH[N], FMT_CH> : public ax::FmtFormatterBase_<FMT_CH> {
	using Base = ax::FmtFormatterBase_<FMT_CH>;

	template<class Context>
	constexpr auto format(const CH (&sz)[N], Context& ctx) const {
		// ax::StrView_<CH> sv(sz);
		// return ctx.out() = sv;
		return ctx.out();		
	}
};

template <>
struct std::formatter<ax::MutStrView8> {
	template<class ParseContext>
	constexpr auto parse(ParseContext& ctx) { return ctx.end(); }
	
	template <class FormatContext>
	constexpr auto format(const ax::MutStrView8 & obj, FormatContext& ctx) const {
		return std::format_to(ctx.out(), "2");
	}
};