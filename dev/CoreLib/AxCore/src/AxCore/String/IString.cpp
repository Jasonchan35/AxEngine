module;

#include "AxCore-pch.h"
module AxCore.IString;

import AxCore.UtfUtil;
import AxCore.PersistString;
import AxCore.SpinLock;
import AxCore.GlobalSingleton;

namespace ax {

template <class T> constexpr void IString_<T>::appendUtf(StrViewA  r) { UtfUtil::append(*this, r); }
template <class T> constexpr void  IString_<T>::appendUtf(StrViewW  r) { UtfUtil::append(*this, r); }
template <class T> constexpr void  IString_<T>::appendUtf(StrView8  r) { UtfUtil::append(*this, r); }
template <class T> constexpr void  IString_<T>::appendUtf(StrView16 r) { UtfUtil::append(*this, r); }
template <class T> constexpr void  IString_<T>::appendUtf(StrView32 r) { UtfUtil::append(*this, r); }


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

template<class T>
PersistString_<T> PersistString_<T>::s_make(StrView_<T> s) {
	return Manager::s_instance()->lookup(s);
}

#define	E(T)	\
/*---- The explicit instantiation ---*/ \
	template class IString_<T>; \
	template class PersistString_<T>; \
//--------
	AX_TYPE_LIST_CHAR(E)
#undef	E

} // namespace
