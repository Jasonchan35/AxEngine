module;
#include "AxCore-pch.h"

export module AxCore.PersistString;
export import AxCore.Formatter;
export import AxCore.Dict;

export namespace ax {

template<class T>
class PersistString_ { // copyable
	using This = PersistString_;
public:
	PersistString_() = default;
	
	static PersistString_	s_make(StrView_<T> s);
	Int			size() const { return _p->size(); }
	
	const StrLit_<T>& strLit() const { return *_p; }
	const T*	c_str() const { return _p->c_str(); }

	StrView_<T> view() const { return *_p; }

	operator StrLit_<T>	() const { return strLit(); }
	operator StrView_<T>() const { return strLit(); }

	bool operator==(const This& r) const { return _p == r._p && view() == r.view(); }
	bool operator==(const StrView_<T>& r) const { return view() == r; }
	
	explicit operator bool() const { return size() > 0; }

	HashInt	onHashInt() const { return HashInt::s_make(strLit()); }
	
	class Manager;
	friend class Manager;
protected:
	PersistString_(const StrLit_<T>* p) : _p(p) {}

	static constexpr StrLit_<T> s_empty_StrLit; 
	const StrLit_<T>*	_p = &s_empty_StrLit;
};

using PersistString   = PersistString_<Char>;
using PersistStringA  = PersistString_<CharA>;
using PersistStringW  = PersistString_<CharW>;
using PersistString16 = PersistString_<Char16>;
using PersistString32 = PersistString_<Char32>;

} // namespace
