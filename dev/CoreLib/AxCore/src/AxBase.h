#pragma once

#include "AX_DETECT_PLATFORM.h"
#include "Base/AX_MACRO.h"

#if AX_COMPILER_CLANG
	#define AX_USE_IMPROT_INSTEAD_INCLUDE 0
#else
	#define AX_USE_IMPROT_INSTEAD_INCLUDE 0
#endif

#if AX_USE_IMPROT_INSTEAD_INCLUDE // use import or #include

import <atomic>;
import <algorithm>;
import <bit>;
import <cassert>;
import <cstdint>;
import <cstdlib>;
import <cctype>;
import <exception>;
import <format>;
import <iostream>;
import <memory>;
import <optional>;
import <source_location>;
import <stdexcept>;
import <string_view>;
import <thread>;
import <tuple>;
import <type_traits>;
import <utility>;

// SSE / AVX
import <xmmintrin.h>;
#if AX_COMPILER_CLANG && AX_OS_WINDOWS
	import <avxintrin.h>;
#else
	import <immintrin.h>;
#endif
//----------

#if AX_OS_WINDOWS
	import <conio.h>;
	import <windows.h>;
#endif

#else // AX_USE_IMPROT_INSTEAD_INCLUDE

#include <atomic>
#include <algorithm>
#include <bit>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cctype>
#include <exception>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <source_location>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

// SSE / AVX
#include <xmmintrin.h>
#if AX_COMPILER_CLANG && AX_OS_WINDOWS
	#include <avxintrin.h>
#else
	#include <immintrin.h>
#endif
//----------

#if AX_OS_WINDOWS
	#include <conio.h>
	#include <windows.h>
#endif

#endif // AX_USE_IMPROT_INSTEAD_INCLUDE

// by windows.h
#ifdef small
#undef small // char
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif
//------
