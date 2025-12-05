module;
#include "AxCore-pch.h"

export module AxCore.Format;
export import AxCore.StrView;

export namespace ax {

// using FormatContext_ = std::format_context;
template<class FMT_CH>
using StdFormatContext_ = std::_Default_format_context<FMT_CH>;
// using FormatContext_ = std::basic_format_context<FormatBackInserter_<FMT_CH>, FMT_CH>;

// using FormatParseContext_ = std::format_parse_context;
template<class FMT_CH>
using StdFormatParseContext_ = std::basic_format_parse_context<FMT_CH>;

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

template<class... ARGS> using FormatString   = FormatString_<Char  , ARGS...>; 
template<class... ARGS> using FormatStringA  = FormatString_<CharA , ARGS...>; 
template<class... ARGS> using FormatStringW  = FormatString_<CharW , ARGS...>; 
template<class... ARGS> using FormatString8  = FormatString_<Char8 , ARGS...>; 
template<class... ARGS> using FormatString16 = FormatString_<Char16, ARGS...>; 
template<class... ARGS> using FormatString32 = FormatString_<Char32, ARGS...>; 

template<class FMT_CH>
using StdFormatArgs_ = std::basic_format_args<StdFormatContext_<FMT_CH>>;

template<class FMT_CH>
using StdFormatter_ = std::formatter<std::basic_string_view<FMT_CH>, FMT_CH>;

template<class FMT_CH> class Format_;

template <class T, class FMT_CH>
class FormatHandler {
public:
	void onFormat(const T & obj, Format_<FMT_CH> & fmt) { obj.onFormat(fmt); }
};

template<class FMT_CH>
class Format_ : public NonCopyable {
	using This = Format_;
public:
	using StdContext   = StdFormatContext_<FMT_CH>;
	using StdFormatter = StdFormatter_<FMT_CH>;

	constexpr Format_(const StdFormatter & formatter_, StdContext & ctx_) : formatter(formatter_), formatContext(ctx_) {}
	
	template<class OBJ>
	constexpr void operator << (const OBJ& obj) {
		FormatHandler<OBJ, FMT_CH> handler;
		handler.onFormat(obj, *this);
	}

	void newline() {
		*this << '\n';
		for (Int i = 0; i < _indent; i++) {
			*this << "  ";
		}
	}

	AX_INLINE Int indent() const { return _indent; }
	AX_INLINE void incIndent() { _indent++; }
	AX_INLINE void decIndent() { _indent--; }
	AX_NODISCARD ScopeObjFunc0<This, &This::decIndent> indentScope() { incIndent(); return this; }

	const StdFormatter& formatter;
	StdContext&   formatContext;
private:
	Int _indent = 0;	
};

} // namespace



