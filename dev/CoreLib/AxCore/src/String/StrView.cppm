export module AxCore.StrView;

#include "AxBase.h"
export import AxCore.BasicType;
export import AxCore.Span;

export namespace ax {

template<CharType T> class MutStrView_;
using MutStrView   = MutStrView_<Char  >;
using MutStrViewA  = MutStrView_<CharA >;
using MutStrViewW  = MutStrView_<CharW >;
using MutStrView8  = MutStrView_<Char8 >;
using MutStrView16 = MutStrView_<Char16>;
using MutStrView32 = MutStrView_<Char32>;

template<CharType T> using StrView_ = MutStrView_<const T>;
using StrView   = StrView_<Char  >;
using StrViewA  = StrView_<CharA >;
using StrViewW  = StrView_<CharW >;
using StrView8  = StrView_<Char8 >;
using StrView16 = StrView_<Char16>;
using StrView32 = StrView_<Char32>;

template <CharType T>
class MutStrView_ : public MutSpan<T> { //Copyable
	using This = MutStrView_;
	using Base = MutSpan<T>;
protected:
	using Base::_data;
	using Base::_size;	
public:
	using CharType = T;
	using MutCharType = std::remove_cv_t<T>;
	
	constexpr MutStrView_() = default;
	constexpr MutStrView_(T* data, Int size) noexcept : Base(data, size) {}

	template<Int N>
	AX_INLINE consteval MutStrView_(T (&sz)[N]) noexcept : Base(sz, N > 0 ? N-1 : 0) {} // consteval only for string literal
};

AX_INLINE consteval StrViewA  operator ""_sv(const CharA * sz, size_t n)  noexcept { return StrViewA (sz, n); }
AX_INLINE consteval StrViewW  operator ""_sv(const CharW * sz, size_t n)  noexcept { return StrViewW (sz, n); }
AX_INLINE consteval StrView8  operator ""_sv(const Char8 * sz, size_t n)  noexcept { return StrView8 (sz, n); }
AX_INLINE consteval StrView16 operator ""_sv(const Char16* sz, size_t n)  noexcept { return StrView16(sz, n); }
AX_INLINE consteval StrView32 operator ""_sv(const Char32* sz, size_t n)  noexcept { return StrView32(sz, n); }

} // namespace
