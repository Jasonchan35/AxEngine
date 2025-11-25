module;

#include "AxPlatform-pch.h"
export module AxPlatform._PCH;

#if !AX_USE_PRECOMPILE_HEADER

export import <algorithm>;
export import <bit>;
export import <cctype>;
export import <cfloat>;
export import <cstdint>;
export import <cstdlib>;
export import <exception>;
export import <format>;
export import <iostream>;
export import <memory>;
export import <mutex>;
export import <optional>;
export import <source_location>;
export import <stdexcept>;
export import <string_view>;
export import <thread>;
export import <tuple>;
export import <type_traits>;
export import <utility>;

// SSE / AVX
#if AX_COMPILER_CLANG && AX_OS_WINDOWS
	export import <xmmintrin.h>;
	export import <avxintrin.h>;
#else
	export import <xmmintrin.h>;
	export import <immintrin.h>;
#endif
//----------

#if AX_OS_WINDOWS
	export import <conio.h>;
	export import <windows.h>;
#endif

#endif // AX_USE_IMPROT_INSTEAD_INCLUDE

export namespace ax {

} // namespace