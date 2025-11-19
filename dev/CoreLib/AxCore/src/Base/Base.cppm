export module AxCore.Base;

#include "AX_DETECT_PLATFORM.h"

import <cstdint>;
import <type_traits>;

export namespace ax {

	class NonCopyable {
	public:
		NonCopyable()				 = default;
		NonCopyable(NonCopyable&& r) = default;
		NonCopyable(const NonCopyable& s)	 = delete; //!< not allow by default
		void operator=(const NonCopyable& s) = delete; //!< not allow by default
	};

	using u8  = std::uint8_t;
	using u16 = std::uint16_t;
	using u32 = std::uint32_t;
	using u64 = std::uint64_t;

	using i8  = std::int8_t;
	using i16 = std::int16_t;
	using i32 = std::int32_t;
	using i64 = std::int64_t;

	using f32  = float;
	using f64  = double;
	using f128 = long double;

	using Int  = i64;
	using UInt = u64;
	
	template <class T>
	concept CON_CharType
		=  std::is_same_v<T, char>
		|| std::is_same_v<T, wchar_t>
		|| std::is_same_v<T, char8_t>
		|| std::is_same_v<T, char16_t>
		|| std::is_same_v<T, char32_t>;
	
}
