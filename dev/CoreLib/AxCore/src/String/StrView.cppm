module;
#include "AxBase.h"

export module AxCore.StrView;

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

enum class StrCase : u8 {
	Ignore,
	Sensitive,
};

template <CharType T>
class MutStrView_ { //Copyable
	using This = MutStrView_;
protected:
	T*  _data = nullptr;
	Int _size = 0;	
public:
	using CharType = T;
	using MView = MutStrView_<T>;
	using CView = MutStrView_<const T>;
	using MutByte =	typename std::conditional_t<std::is_const_v<T>, const Byte, Byte>;
	using std_string_view = std::basic_string_view<std::remove_cv_t<T>>;
	
	constexpr MutStrView_() = default;
	constexpr MutStrView_(T* data, Int size) noexcept : _data(data), _size(size) {}

	template<Int N>
	AX_INLINE constexpr MutStrView_(T (&sz)[N]) noexcept : _data(sz), _size(N > 0 ? N-1 : 0) {}

	constexpr std_string_view to_string_view() const noexcept { return std_string_view(_data, _size); }
	constexpr operator std_string_view() const noexcept { return to_string_view(); }

	AX_INLINE constexpr       T* data()       noexcept { return _data; }
	AX_INLINE constexpr const T* data() const noexcept { return _data; }
	AX_INLINE constexpr Int      size() const noexcept { return _size; }
	AX_INLINE constexpr Int	     sizeInBytes() const noexcept { return _size * ax_sizeof<T>; }

	AX_INLINE	constexpr static MView	s_fromMutByteSpan	(MutByteSpan	from) noexcept	{ return MView(reinterpret_cast<T*>(from.data()), from.sizeInBytes() / ax_sizeof<T>); }
	AX_INLINE	constexpr static CView	s_fromByteSpan		(ByteSpan		from) noexcept	{ return CSpan(reinterpret_cast<T*>(from.data()), from.sizeInBytes() / ax_sizeof<T>); }

	AX_INLINE	constexpr void			fromMutByteSpan		(MutByteSpan	from) noexcept	{ *this = s_fromMutByteSpan(from); }
	AX_INLINE	constexpr void			fromByteSpan		(   ByteSpan	from) noexcept	{ *this = s_fromByteSpan(from); }

	AX_INLINE	constexpr MutByteSpan	toMutByteSpan		()       noexcept { return MutByteSpan(reinterpret_cast<   MutByte*>(_data), sizeInBytes()); }
	AX_INLINE	constexpr    ByteSpan	toByteSpan			() const noexcept { return    ByteSpan(reinterpret_cast<const Byte*>(_data), sizeInBytes()); }

	constexpr bool equals(const This & r, StrCase sc) const noexcept;
	
	constexpr bool operator==(const This & r) const noexcept { return  equals(r, StrCase::Sensitive); }
	constexpr bool operator!=(const This & r) const noexcept { return !equals(r, StrCase::Sensitive); }

	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()		 noexcept	{ return _data; }
	constexpr CIter	begin	() const noexcept	{ return _data; }
	constexpr  Iter	end		()		 noexcept	{ return _data + _size; }
	constexpr CIter	end		() const noexcept	{ return _data + _size; }
};

template <CharType T> AX_INLINE
constexpr bool MutStrView_<T>::equals(const This& r, StrCase sc) const noexcept {
	if (_data == r._data && _size == r._size) return true;
	if (sc == StrCase::Ignore) {
		for (Int i = 0; i < _size; ++i) {
			if (std::toupper(_data[i]) != std::toupper(r._data[i]))
				return false;
		}
	} else {
		for (Int i = 0; i < _size; ++i) {
			if (_data[i] != r._data[i])
				return false;
		}
	}
	return true;
}

AX_INLINE consteval StrViewA  operator ""_sv(const CharA * sz, size_t n)  noexcept { return StrViewA (sz, n); }
AX_INLINE consteval StrViewW  operator ""_sv(const CharW * sz, size_t n)  noexcept { return StrViewW (sz, n); }
AX_INLINE consteval StrView8  operator ""_sv(const Char8 * sz, size_t n)  noexcept { return StrView8 (sz, n); }
AX_INLINE consteval StrView16 operator ""_sv(const Char16* sz, size_t n)  noexcept { return StrView16(sz, n); }
AX_INLINE consteval StrView32 operator ""_sv(const Char32* sz, size_t n)  noexcept { return StrView32(sz, n); }

template<class T> AX_INLINE
constexpr StrView_<T> StrView_make(std::basic_string_view<T> src) { return StrView_<T>(src.data(), src.size()); }

template<class T, Int N> AX_INLINE
constexpr StrView_<T> StrView_make(T (&sz)[N]) { return StrView_<T>(sz, N > 0 ? N-1 : 0); }

} // namespace
