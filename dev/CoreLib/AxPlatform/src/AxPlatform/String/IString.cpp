module;

#include "AxPlatform-pch.h"
module AxPlatform.IString;

import AxPlatform.UtfUtil;

namespace ax {

template <class T> constexpr void IString_<T>::appendUtf(StrViewA  r) { UtfUtil::append(*this, r); }
template <class T> constexpr void  IString_<T>::appendUtf(StrViewW  r) { UtfUtil::append(*this, r); }
template <class T> constexpr void  IString_<T>::appendUtf(StrView8  r) { UtfUtil::append(*this, r); }
template <class T> constexpr void  IString_<T>::appendUtf(StrView16 r) { UtfUtil::append(*this, r); }
template <class T> constexpr void  IString_<T>::appendUtf(StrView32 r) { UtfUtil::append(*this, r); }

#define	E(T)	\
/*---- The explicit instantiation ---*/ \
	template class IString_<T>; \
//--------
	AX_TYPE_LIST_CHAR(E)
#undef	E

} // namespace
