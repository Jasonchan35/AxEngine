#pragma once

#include "AxCore/Platform/AX_DETECT_PLATFORM.h"

#include <xmmintrin.h>
#include <immintrin.h>

#if AX_USE_PRECOMPILE_HEADER

#include <algorithm>
#include <bit>
#include <cctype>
#include <cfloat>
#include <chrono>
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

#endif // AX_USE_PRECOMPILE_HEADER

// header must use #include instead of import
#include <atomic>
#include <cassert>
#include <cmath>

// SSE / AVX
#include <xmmintrin.h>
#include <immintrin.h>
// #include <avxintrin.h>

#if AX_OS_WINDOWS
	#include "AxCore/Platform/OS_Windows.h"
#else
	#include <uuid/uuid.h>
	#include <dirent.h>
	#include <sys/stat.h>
#endif

#include "AxCore/Base/AX_MACRO.h"
#include "AxCore/Reflection/AxRtti.h"
