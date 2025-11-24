module;
#include "AxCore-pch.h"

export module AxCore.Format;
export import AxCore.StrView;

export namespace ax {

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
	constexpr FormatStringT_(StrLit_<T> view) : _view(view) {}
//	constexpr FormatStringT_(const IString_<T> & str) : FormatStringT_(str.view()) {}

	constexpr StrView_<T> get() const { return _view; }
	constexpr Int size() const { return _view.size(); } 
	
private:
	template<class PARAM>
	consteval void _check_format(PARAM && param) {
		std_format_string checker(AX_FORWARD(param));
	}
	
	StrView_<T> _view;
};

template<class T, class... ARGS>
using FormatString_ = FormatStringT_<T, std::type_identity_t<ARGS>...>;


template<class FMT_CH>
using FormatArgs_ = std::basic_format_args<FormatContext_<FMT_CH>>;

//--- Formatter
template<class FMT_CH>
struct FormatterBase_ : public std::formatter<std::basic_string_view<FMT_CH>, FMT_CH> {
	using Base = std::formatter<std::basic_string_view<FMT_CH>, FMT_CH>;
	
	template<class Context>
	constexpr auto parse(Context& ctx) { return Base::parse(ctx); }
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

	constexpr Format_(const Formatter & formatter_, Context & ctx_) : formatter(formatter_), formatContext(ctx_) {}

	AX_INLINE constexpr void append(StrView_<T> view) {
		formatter.format(view.to_string_view(), formatContext);
	}

	AX_INLINE constexpr void operator << (StrView_<T> view) { append(view); }

	const Formatter& formatter;
	Context&   formatContext;
};


template<class OBJ, class FMT_CH>
concept Format_HasOnFormat_ = requires(const OBJ& obj, Format_<FMT_CH> & fmt) {
//	{ obj.onFormat(fmt) } -> std::same_as<void>;
	true;
};

} // namespace



