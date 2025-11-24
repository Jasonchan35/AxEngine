module;

#include "AxCore-pch.h"
module AxCore.IString;

import AxCore.UtfUtil;

namespace ax {

template <class T> constexpr void IString_<T>::appendUtf(StrViewA  src) { UtfUtil::append(*this, src); }
template <class T> constexpr void IString_<T>::appendUtf(StrViewW  src) { UtfUtil::append(*this, src); }
template <class T> constexpr void IString_<T>::appendUtf(StrView8  src) { UtfUtil::append(*this, src); }
template <class T> constexpr void IString_<T>::appendUtf(StrView16 src) { UtfUtil::append(*this, src); }
template <class T> constexpr void IString_<T>::appendUtf(StrView32 src) { UtfUtil::append(*this, src); }

#define	E(T)	\
/*---- The explicit instantiation ---*/ \
	template class IString_<T>; \
//--------
	AX_TYPE_LIST_CHAR(E)
#undef	E

} // namespace
