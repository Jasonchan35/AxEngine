module;
#include "AxPlatform-pch.h"

export module AxPlatform.Format;
export import AxPlatform.StrView;

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

} // namespace



