export module AxCore.StrView;

#include "Base/AX_MACRO.h"

export import AxCore.BasicType;

export namespace ax {

template<CharType T> class MutStrView_;
template<CharType T> using StrView_ = MutStrView_<const T>;

using StrView8  = StrView_<Char8 >;
using StrView16 = StrView_<Char16>;
using StrView32 = StrView_<Char32>;
using StrViewW  = StrView_<CharW >;
using StrView   = StrView_<Char  >;

template <CharType T>
class MutStrView_ {
	//Copyable
	using This = MutStrView_;

protected:
	T*  _data = nullptr;
	Int _size = 0;

public:
	using CharType = T;
	constexpr MutStrView_(T* data, Int size) noexcept : _data(data), _size(size) {}

	constexpr bool inBound(Int i) const noexcept { return (i >= 0 && i < _size); } 
	constexpr T & at(Int i) { if (!inBound(i)) throw Error_IndexOutOfRange(); return _data[i]; }
	constexpr T & operator[](Int i) { return at(i); }
	
};

inline StrView8 operator ""_sv8(const Char8* sz, size_t size) { return StrView8(sz, size); }
inline StrView  operator ""_sv(const char* sz, size_t size) { return StrView(ax_char_pointer(sz), size); }

} // namespace
