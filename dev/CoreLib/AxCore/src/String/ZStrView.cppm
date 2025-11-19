export module AxCore.ZStrView;

#include "AxBase.h"
export import AxCore.StrView;

export namespace ax {

template<CharType T> class MutZStrView_;
template<CharType T> using ZStrView_ = MutZStrView_<const T>;

using MutZStrView	= MutZStrView_<Char>;
using MutZStrViewA	= MutZStrView_<CharA>;
using MutZStrViewW	= MutZStrView_<CharW>;
using MutZStrView8	= MutZStrView_<Char8>;
using MutZStrView16	= MutZStrView_<Char16>;
using MutZStrView32	= MutZStrView_<Char32>;

using ZStrView		= ZStrView_<Char>;
using ZStrViewA		= ZStrView_<CharA>;
using ZStrViewW		= ZStrView_<CharW>;
using ZStrView8		= ZStrView_<Char8>;
using ZStrView16	= ZStrView_<Char16>;
using ZStrView32	= ZStrView_<Char32>;

// Null-terminated string, should use StrLit or StrView if possible
template<CharType T>
class MutZStrView_ : public MutStrView_<T> {
	using This = MutZStrView_;
	using Base = MutStrView_<T>;
protected:
	using Base::_data;
	using Base::_size;
public:
	using View = MutZStrView_<const T>;

	AX_INLINE constexpr MutZStrView_() = default;
	AX_INLINE constexpr MutZStrView_(T* sz, Int size) { Base::setPtr(sz, size); }

	template<Int N>
	AX_INLINE constexpr MutZStrView_(T (&sz)[N]) { Base::setPtr(sz, N > 0 ? N-1 : 0); }
	
	View		constView() const { return View(_data, _size); }

	const T*	c_str() const { return _size ? _data : &_kZero; }

private:
	static constexpr T _kZero = 0;
};

} // namespace