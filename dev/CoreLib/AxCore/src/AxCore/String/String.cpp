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



#define	E(T)	\
/*---- The explicit instantiation ---*/ \
	template class IString_<T>; \
	template class PersistString_<T>; \
//--------
	AX_TYPE_LIST_CHAR(E)
#undef	E

} // namespace
