module;


module AxCore.IString;

import AxCore.UtfUtil;
import AxCore.SpinLock;
import AxCore.String;

namespace ax {

template <class T> constexpr void IString_<T>::appendUtf(StrViewA  r) { UtfUtil::append(*this, r); }
template <class T> constexpr void IString_<T>::appendUtf(StrViewW  r) { UtfUtil::append(*this, r); }
template <class T> constexpr void IString_<T>::appendUtf(StrView8  r) { UtfUtil::append(*this, r); }
template <class T> constexpr void IString_<T>::appendUtf(StrView16 r) { UtfUtil::append(*this, r); }
template <class T> constexpr void IString_<T>::appendUtf(StrView32 r) { UtfUtil::append(*this, r); }

template <class T> constexpr 
Int IString_<T>::replaceAll(CView from, CView to, StrCase sc) {
	if (!from) return 0;

	TempString_<T> tmpStr(*this);
	clear();

	Int replaced = 0;
	auto v = tmpStr.view();

	while (v) {
		auto f = v.find(from, sc);
		if (!f) break;

		append(v.slice(0, f.value()));
		append(to);
		v = v.sliceFrom(f.value() + from.size());
		replaced++;
	}

	append(v);
	return replaced;
}


#define	E(T)	\
/*---- The explicit instantiation ---*/ \
	template class IString_<T>; \
//--------
	AX_TYPE_LIST_CHAR(E)
#undef	E

} // namespace
