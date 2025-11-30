#pragma once

#include "AxCore/Platform/AX_DETECT_PLATFORM.h"
#include "AxCore/Base/AX_MACRO.h"

#include <xmmintrin.h>
#include <immintrin.h>

// header must use #include instead of import
#include <atomic>
#include <cassert>
#include <cmath>

// SSE / AVX
#include <xmmintrin.h>
#include <immintrin.h>
// #include <avxintrin.h>

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
#include <random>
#include <source_location>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

#if AX_OS_WINDOWS
	#include "AxCore/Platform/AX_OS_WINDOWS.h"
	#include <conio.h>
	#include <comdef.h> //Com error
#else
	#include <uuid/uuid.h>
#endif

#endif // AX_USE_PRECOMPILE_HEADER