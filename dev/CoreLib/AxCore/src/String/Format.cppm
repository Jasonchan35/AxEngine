module;
#include "AxCore-pch.h"

export module AxCore.Format;

export import AxCore.String;
export import AxCore.UtfUtil;

import AxCore.Debug;

export namespace ax {

template<class OUT_CH>	
struct FormatBackInserter_ {
	using This = FormatBackInserter_;
	using OutIString = IString_<OUT_CH>;
	
	using difference_type = ptrdiff_t;

	constexpr explicit FormatBackInserter_(OutIString& s) noexcept : _s(&s) {}

	constexpr This& operator=(const OUT_CH&  ch) { _s->appendChar(ch); return *this; }
	constexpr This& operator=(      OUT_CH&& ch) { _s->appendChar(std::move(ch)); return *this; }

	constexpr       This& operator*()       noexcept { return *this; }
	constexpr const This& operator*() const noexcept { return *this; }
	
	constexpr This& operator++() noexcept { return *this; }
	constexpr This  operator++(int) noexcept { return *this; }

protected:
	OutIString* _s = nullptr;
};

// using FormatContext_ = std::format_context;
template<class FMT_CH>
using FormatContext_ = std::_Default_format_context<FMT_CH>;
// using FormatContext_ = std::basic_format_context<FormatBackInserter_<FMT_CH>, FMT_CH>;

// using FormatParseContext_ = std::format_parse_context;
template<class FMT_CH>
using FormatParseContext_ = std::basic_format_parse_context<FMT_CH>;

// template<class... ARGS> using FormatString_ = std::format_string<ARGS...>;

template<class T, class... ARGS>
class FormatStringT_ : public NonCopyable {
	using std_format_string = std::basic_format_string<T, std::type_identity_t<ARGS>...>;		

public:
	template<Int N> inline
	consteval FormatStringT_(const T (&sz)[N]) : _view(sz) {
		_check_format(sz);
	}
	
	constexpr FormatStringT_(StrView_<T> view) : _view(view) {}
	constexpr FormatStringT_(const IString_<T> & str) : FormatStringT_(str.view()) {}

	constexpr StrView_<T> get() const { return _view; }
	constexpr Int size() const { return _view.size(); } 
	
private:
	template<class PARAM>
	consteval void _check_format(PARAM && param) {
		#if !AX_COMPILER_CLANG
		std_format_string checker(AX_FORWARD(param));
		#endif
	}
	
	StrView_<T> _view;
};

template<class T, class... ARGS>
using FormatString_ = FormatStringT_<T, std::type_identity_t<ARGS>...>;


template<class FMT_CH>
using FormatArgs_ = std::basic_format_args<FormatContext_<FMT_CH>>;

template<class T, class ... ARGS> 
AX_INLINE void ax_format_to_internal(IString_<T> & output, FormatString_<T, ARGS...> && fmt, ARGS&&... args) {
	FormatArgs_<T> format_args = std::make_format_args<FormatContext_<T>>(args...);
	auto fmt_sv = fmt.get().to_string_view();
	try {
		output.reserve(output.size() + fmt.size() + format_args._Estimate_required_capacity());
		std::vformat_to(FormatBackInserter_<T>(output), fmt_sv, format_args);
			
	} catch (std::exception& e) {
		auto msg  = std::format("Exception in format={}, {})", fmt_sv, e.what());
		Debug::_internal_logError(msg.c_str());
		throw Error_Format();
	}
}

template<class... ARGS> AX_INLINE
void FmtTo(IString_<CharA> & output, FormatString_<CharA, ARGS...> && fmt, ARGS&&... args) { return ax_format_to_internal<CharA>(output, AX_FORWARD(fmt), AX_FORWARD(args)...); }
template<class... ARGS> AX_INLINE
void FmtTo(IString_<CharW> & output, FormatString_<CharW, ARGS...> && fmt, ARGS&&... args) { return ax_format_to_internal<CharW>(output, AX_FORWARD(fmt), AX_FORWARD(args)...); }

template<class... ARGS> AX_INLINE
StringA Fmt(FormatString_<CharA, ARGS...> && fmt, ARGS&&... args) { StringA str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }

template<class... ARGS> AX_INLINE
StringW Fmt(FormatString_<CharW, ARGS...> && fmt, ARGS&&... args) { StringW str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }

//--- Formatter
template<class FMT_CH>
struct FormatterBase_ : public std::formatter<std::basic_string_view<FMT_CH>, FMT_CH> {
	using Base = std::formatter<std::basic_string_view<FMT_CH>, FMT_CH>;
	
	template<class Context>
	constexpr auto parse(Context& ctx) { return Base::parse(ctx); }
};

template<class OBJ>
concept Format_HasOnFormat_ = requires(const OBJ& obj) {
	{ obj.onFormat };
};

template<class OBJ>
concept Format_HasOnParse_ = requires(const OBJ& obj) {
	{ OBJ::onFormatParse };
};

template<class T>
class Format_ : public NonCopyable {
public:
	using Context   = FormatContext_<T>;
	using Formatter = FormatterBase_<T>;

	constexpr Format_(const Formatter & formatter_, Context & ctx_) : formatter(formatter_), ctx(ctx_) {}

	AX_INLINE constexpr void append(StrView_<T> view) {
		formatter.format(view.to_string_view(), ctx);
	}

	template<class ... ARGS>
	AX_INLINE constexpr void format(FormatString_<T, ARGS...> fmt, ARGS&&... args) {
		TempString_<T> tmp;
		FmtTo(tmp, AX_FORWARD(fmt), AX_FORWARD(args)...);
		append(tmp);
	}

	const Formatter& formatter;
	Context&   ctx;
};

} // namespace

//----- global namespace ----------

// Wrapper to CustomClass::onFormat()
template<class OBJ, class FMT_CH> requires ax::Format_HasOnFormat_<OBJ>
struct std::formatter<OBJ, FMT_CH> : public ax::FormatterBase_<FMT_CH> {
	using Base = ax::FormatterBase_<FMT_CH>;

	constexpr auto parse(ax::FormatParseContext_<FMT_CH>& ctx) {
		if constexpr (ax::Format_HasOnParse_<OBJ>) {
			OBJ::onFormatParse(ctx);
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

template <class CH, class FMT_CH>
struct std::formatter<ax::MutStrView_<CH>, FMT_CH> : public ax::FormatterBase_<FMT_CH> {
	using Base = ax::FormatterBase_<FMT_CH>;
	constexpr auto format(const ax::MutStrView_<CH>& obj, ax::FormatContext_<FMT_CH>& ctx) const {
		return Base::format(obj, ctx);
	}
};

template <class CH, class FMT_CH>
struct std::formatter<ax::IString_<CH>, FMT_CH> : public ax::FormatterBase_<FMT_CH> {
	using Base = ax::FormatterBase_<FMT_CH>;
	auto format(const ax::IString_<CH>& obj, ax::FormatContext_<FMT_CH>& ctx) const {
		return Base::format(obj, ctx);
	}
};

template <class CH, ax::Int N, class FMT_CH>
struct std::formatter<ax::String_<CH, N>, FMT_CH> : public ax::FormatterBase_<FMT_CH> {
	using Base = ax::FormatterBase_<FMT_CH>;
	auto format(const ax::String_<CH, N>& obj, ax::FormatContext_<FMT_CH>& ctx) const {
		return Base::format(obj, ctx);
	}
};

template <ax::CharType CH, size_t N, class FMT_CH> requires (!std::is_same_v<CH, FMT_CH>)
struct std::formatter<CH[N], FMT_CH> : public ax::FormatterBase_<FMT_CH> {
	using Base = ax::FormatterBase_<FMT_CH>;
	constexpr auto format(const CH (&sz)[N], ax::FormatContext_<FMT_CH>& ctx) const {
		ax::TempString_<FMT_CH> tmp;
		ax::UtfUtil::convert(tmp, ax::StrView_<CH>(sz));
		return Base::format(tmp, ctx);
	}
};
