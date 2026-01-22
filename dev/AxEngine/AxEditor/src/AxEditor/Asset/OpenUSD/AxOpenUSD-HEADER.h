#pragma once

AX_VC_WARNING_PUSH()
AX_VC_WARNING_DISABLE(4244) // '=': conversion from 'int' to 'unsigned short', possible loss of data
AX_VC_WARNING_DISABLE(4305) // 'argument': truncation from 'double' to 'float'
AX_VC_WARNING_DISABLE(5219) // implicit conversion from '__int64' to 'double', possible loss of data
AX_VC_WARNING_DISABLE(5215) // 'a_value' a function parameter with a volatile qualified type is deprecated in C++20
AX_VC_WARNING_DISABLE(5220) // 'tbb::internal::atomic_impl<T>::converter<volatile const value_type>::value

#ifndef NOMINMAX
	#define NOMINMAX 1 // no min and max macro in windows.h
#endif

#include <pxr/usd/usd/stage.h>

AX_VC_WARNING_POP()