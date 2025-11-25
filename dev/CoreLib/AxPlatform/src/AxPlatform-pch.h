#pragma once

#include "AxPlatform/Detect/AX_DETECT_PLATFORM.h"
#include "AxPlatform/Base/AX_MACRO.h"

// header must use #include instead of import
#include <atomic>
#include <cassert>
#include <cmath>

// SSE / AVX
#if AX_COMPILER_CLANG && AX_OS_WINDOWS
	#include <xmmintrin.h>
	#include <avxintrin.h>
#else
	#include <xmmintrin.h>
	#include <immintrin.h>
#endif
//---- end of SSE / AVX ------


#if AX_USE_PRECOMPILE_HEADER

#include <algorithm>
#include <bit>
#include <cctype>
#include <cfloat>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <source_location>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

#if AX_OS_WINDOWS
	#include <conio.h>
	#include "AxPlatform/Base/AX_WINDOWS.h"
#endif

#endif // AX_USE_PRECOMPILE_HEADER