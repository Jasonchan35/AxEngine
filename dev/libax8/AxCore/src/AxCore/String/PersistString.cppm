module;

export module AxCore.PersistString;
export import AxCore.Formatter;

export import AxCore.SpinLock;
export import AxCore.GlobalSingleton;
export import AxCore.Dict;


export namespace ax {

template<class T>
class PersistString_Data : public NonCopyable {
public:
	using StrLitType = StrLit_<T>;
	StrLit_<T>	str;
	HashInt		hash;
};

template<class T>
class PersistString_ { // copyable
	using This = PersistString_;
public:
	using Data = PersistString_Data<T>;

	constexpr PersistString_() = default;
	
	static This s_make(StrView_<T> s);
	AX_INLINE constexpr Int size() const { return strLit().size(); }
	
	AX_INLINE constexpr StrLit_<T>  strLit() const { return _data ? _data->str : StrLit_<T>(); }
	AX_INLINE constexpr const T*	c_str() const { return strLit().c_str(); }

	AX_INLINE constexpr StrView_<T> view() const { return strLit(); }

	AX_INLINE constexpr operator StrLit_<T>	() const { return strLit(); }
	AX_INLINE constexpr operator StrView_<T>() const { return strLit(); }

	AX_INLINE constexpr bool operator==(const This& r) const { return _data == r._data; }
	AX_INLINE constexpr bool operator==(const StrView_<T>& r) const { return view() == r; }
	
	AX_INLINE constexpr explicit operator bool() const { return size() > 0; }

	AX_INLINE constexpr HashInt	onHashInt() const { return _data->hash; }
	
	class Manager;
	friend class Manager;
protected:
	constexpr PersistString_(const Data* data) : _data(data) {}
	const Data*	_data = nullptr;
};

using PersistString   = PersistString_<Char>;
using PersistStringA  = PersistString_<CharA>;
using PersistStringW  = PersistString_<CharW>;
using PersistString8  = PersistString_<Char8>;
using PersistString16 = PersistString_<Char16>;
using PersistString32 = PersistString_<Char32>;

} // namespace
