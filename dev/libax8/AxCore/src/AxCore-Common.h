#pragma once

#include "AxCore/Platform/AX_DETECT_PLATFORM.h"

#if  1 // AX_USE_PRECOMPILE_HEADER

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
#include <string>
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
//	#include "AxCore/Platform/AX_OS_WINDOWS.h"
#else
	#include <uuid/uuid.h>
	#include <dirent.h>
	#include <sys/stat.h>
#endif

#include "AxCore/Base/AX_MACRO.h"
#include "AxCore/Reflection/Rtti_MACRO.h"
#include "AxCore/Color/ColorType_MACRO.h"
