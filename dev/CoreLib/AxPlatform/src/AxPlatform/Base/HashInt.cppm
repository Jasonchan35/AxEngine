module;
#include "AxPlatform-pch.h"
export module AxPlatform.HashInt;
export import AxPlatform.BasicType;
export import AxPlatform.String;

export namespace ax {

template<class VALUE>
class HashInt_ {
	using This = HashInt_;
public:
	using Value = VALUE;
	Value	value;

	static constexpr This s_create(const Value& v) { return HashInt_(v); }
private:
	constexpr HashInt_(const Value& v) : value(v) {}
};

using HashInt32 = HashInt_<u32>;
using HashInt64 = HashInt_<u64>;
using HashInt   = HashInt64;

struct HashUtil {
public:
	template<class T> static constexpr HashInt64 compute64(Span<T>   data) { return HashInt64::s_create(_fnv1a_64(data.data(), data.size())); }
	template<class T> static constexpr HashInt32 compute32(Span<T>   data) { return HashInt32::s_create(_fnv1a_32(data.data(), data.size())); }
	template<class T> static constexpr HashInt64 compute64(StrView_<T> sv) { return HashInt64::s_create(_fnv1a_64(  sv.data(),   sv.size())); }
	template<class T> static constexpr HashInt32 compute32(StrView_<T> sv) { return HashInt32::s_create(_fnv1a_32(  sv.data(),   sv.size())); }
	
private:
	// FNV-1a hash 32 bit
	template<class T> requires (std::is_integral_v<T> && sizeof(T) <= 4)
	static constexpr u32 _fnv1a_32(const T* s, Int size) {
		u32 hash = 2166136261u;
		for (Int i = 0; i < size; i++) {
			hash ^= static_cast<u32>(s[i]) * 16777619u;
		}
		return hash;
	}

	// FNV-1a hash 64 bit
	template<class T> requires std::is_integral_v<T>
	static constexpr u64 _fnv1a_64(const T* s, Int size) {
		u64 hash = 14695981039346656037ull;
		for (Int i = 0; i < size; i++) {
			hash ^= static_cast<u64>(s[i]) * 1099511628211ull;
		}
		return hash;
	}
	
};

template<class OBJ> struct HashInt_Handler {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& obj) { return obj.onHashInt(); }
};

template<class OBJ> AX_INLINE constexpr
HashInt HashInt_get(const OBJ& v) { return HashInt_Handler<OBJ>().onHashInt(v); }

template<class OBJ> requires std::is_enum_v<OBJ>
struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) { return HashInt::s_create(static_cast<u64>(v)); }
};

template<class OBJ> requires std::is_fundamental_v<OBJ>
struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) { return HashUtil::fnv1a_64(static_cast<u64>(v)); }
};

template <class OBJ>
requires std::is_convertible_v<OBJ, StrViewA>
      || std::is_convertible_v<OBJ, StrViewW>
      || std::is_convertible_v<OBJ, StrView8>
      || std::is_convertible_v<OBJ, StrView16>
      || std::is_convertible_v<OBJ, StrView32>

struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) {
		       if constexpr (std::is_convertible_v<OBJ, StrViewA >) { return HashUtil::fnv1a_64(StrViewA( v).toByteSpan());
		} else if constexpr (std::is_convertible_v<OBJ, StrViewW >) { return HashUtil::fnv1a_64(StrViewW( v).toByteSpan());
		} else if constexpr (std::is_convertible_v<OBJ, StrView8 >) { return HashUtil::fnv1a_64(StrView8( v).toByteSpan());
		} else if constexpr (std::is_convertible_v<OBJ, StrView16>) { return HashUtil::fnv1a_64(StrView16(v).toByteSpan());
		} else if constexpr (std::is_convertible_v<OBJ, StrView32>) { return HashUtil::fnv1a_64(StrView32(v).toByteSpan());
		} else { static_assert(false); }
		
	}
};

template <class OBJ> requires std::is_convertible_v<OBJ, StrViewW>
struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) { return HashUtil::fnv1a_64(StrViewW(v).span()); }
};

template <class OBJ> requires std::is_convertible_v<OBJ, StrView8>
struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) { return HashUtil::fnv1a_64(StrView8(v).span()); }
};

template <class OBJ> requires std::is_convertible_v<OBJ, StrView16>
struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) { return HashUtil::fnv1a_64(StrView16(v).span()); }
};

template <class OBJ> requires std::is_convertible_v<OBJ, StrView32>
struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) { return HashUtil::fnv1a_64(StrView32(v).span()); }
};

} // namespace