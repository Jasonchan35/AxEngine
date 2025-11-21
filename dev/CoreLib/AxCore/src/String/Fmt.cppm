export module AxCore.Fmt;

#include "AxBase.h"

export import AxCore.String;
import AxCore.Debug;
import <format>;

export namespace ax {

template<class CH>
class Fmt_FStringBackInserter {
	using This = Fmt_FStringBackInserter;
public:
	using difference_type = ptrdiff_t;

	constexpr explicit Fmt_FStringBackInserter(IString_<CH>& s) noexcept : _s(&s) {}

	This& operator=(const CH&  ch) { _s->appendChar(ch); return *this; }
	This& operator=(      CH&& ch) { _s->appendChar(std::move(ch)); return *this; }

	[[nodiscard]] constexpr This& operator*() noexcept { return *this; }
	constexpr This& operator++() noexcept { return *this; }
	constexpr This operator++(int) noexcept { return *this; }

protected:
	IString_<CH>* _s = nullptr;
};

template <class CH>
AX_INLINE Fmt_FStringBackInserter<CH> Fmt_BackInserter(IString_<CH>& s) { return Fmt_FStringBackInserter<CH>(s); }


// using Fmt_Context = std::format_context;
// using Fmt_ParseContext = std::format_parse_context;
// template<class... ARGS> using Fmt_FormatString = std::format_string<ARGS...>;

template<class CH> using Fmt_Context = std::_Default_format_context<CH>;
template<class CH> using Fmt_ParseContext = std::basic_format_parse_context<CH>;
template<class CH, class... ARGS> using Fmt_FormatString = std::basic_format_string<CH, std::type_identity_t<ARGS>...>;

template<class CH, class... ARGS>
inline void FmtTo_(IString_<CH>& outStr, const Fmt_FormatString<CH, ARGS...>& formatString, ARGS&&... args) {
	try {
		std::format_to(Fmt_BackInserter<CH>(outStr), formatString, std::forward<ARGS>(args)...);
	} catch (std::exception& e) {
		auto msg  = std::format("Exception in Fmt {} formatString={}", e.what(), formatString.get());
		Debug::_internal_logError(msg.c_str());
	}
}

template<class CH, class... ARGS> inline
String_<CH> Fmt_(const Fmt_FormatString<CH, ARGS...>& formatString, ARGS&&... args) {
	String_<CH> outStr;
	FmtTo_(outStr, AX_FORWARD(formatString), AX_FORWARD(args)...);
	return outStr;
}

} // namespace
