export module AxCore.StrView;

#include "AxBase.h"
export import AxCore.BasicType;
export import AxCore.Span;

export namespace ax {

template<CharType T> class MutStrView_;
template<CharType T> using StrView_ = MutStrView_<const T>;

using StrView8  = StrView_<Char8 >;
using StrView16 = StrView_<Char16>;
using StrView32 = StrView_<Char32>;
using StrViewW  = StrView_<CharW >;
using StrView   = StrView_<Char  >;

template <CharType T>
class MutStrView_ : public MutSpan<T> { //Copyable
	using This = MutStrView_;
	using Base = MutSpan<T>;
protected:
	using Base::_data;
	using Base::_size;	
public:
	using CharType = T;
	constexpr MutStrView_() = default;
	constexpr MutStrView_(T* data, Int size) noexcept : Base(data, size) {}
};

inline StrView8 operator ""_sv8(const Char8* sz, size_t size) { return StrView8(sz, size); }
inline StrView  operator ""_sv(const char* sz, size_t size) { return StrView(ax_char8_from_charA_pointer(sz), size); }

} // namespace
