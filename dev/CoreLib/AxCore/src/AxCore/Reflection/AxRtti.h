#pragma once

#include "AxMetaType.h"

#define AX_RTTI_CLASS(T, BASE) \
AX_TYPE_INFO(T, BASE) \
public: \
static  Rtti* s_rtti ()				{ return rttiOf<T>(); } \
virtual Rtti* rtti() const override	{ return rttiOf<T>(); } \
private: \
//-----------