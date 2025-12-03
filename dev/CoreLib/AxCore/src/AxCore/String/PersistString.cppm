module;
#include "AxCore-pch.h"

export module AxCore.PersistString;
export import AxCore.StrView;
export import AxCore.Formatter;
export import AxCore.Dict;

import AxCore.SpinLock;
import AxCore.GlobalSingleton;


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


template<class T>
class PersistString_<T>::Manager : public NonCopyable {
	using This = Manager;
public:
	Manager() {
		ax_default_allocator(); // ensure allocator create before this class
	}

	using PersistStr = PersistString_<T>;
	
	using Key   = String_<T>;
	using Value = StrLit_<T>;

	class MTData {
	public:
		Dict<Key, Value> dict;
	};
	Thread::SpinLockProtected<MTData> _mtData;

	static This* s_instance() {
		static GlobalSingleton<This> s;
		return s.ptr();
	}

	static StrLit_<T>* s_emptyStrLit() {
		static StrLit_<T> s;
		return &s;
	}

	PersistStr lookup(StrView_<T> strview) {
		if (!strview) return s_emptyStrLit();

		auto mt = _mtData.scopedLock();

		if (auto* value = mt->dict.find(strview)) {
			return PersistStr(value);
		}
		
		auto& node = mt->dict.addNode(strview);
		auto& key = node.key();
		node.value = StrLit_<T>(key.data(), key.size());

		return PersistStr(&node.value);
	}
};

template<class T> inline
PersistString_<T> PersistString_<T>::s_make(StrView_<T> s) {
	return Manager::s_instance()->lookup(s);
}



} // namespace
