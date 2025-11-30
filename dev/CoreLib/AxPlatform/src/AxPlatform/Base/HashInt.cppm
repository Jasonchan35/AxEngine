module;
#include "AxPlatform-pch.h"
export module AxPlatform.HashInt;
export import AxPlatform.BasicType;
export import AxPlatform.String;

export namespace ax {

template<class VALUE>
class HashInt_ {
	using This = HashInt_;
	constexpr HashInt_(const VALUE& v) : value(v) {} // private
public:
	using Value = VALUE;
	Value	value;

	constexpr HashInt_ operator^(const This& other) const { return value ^ other.value; }

	static constexpr This s_make(const Value& v) { return HashInt_(v); }

	template<class T> requires Type_IsInt<T>
	static constexpr This s_make(Span<T>     span) { return _s_fnv1a_hash(span); }
	
	template<class T> requires Type_IsChar<T>
	static constexpr This s_make(StrView_<T> view) { return _s_fnv1a_hash(view.span()); }

private:	
	template<class T> requires std::is_integral_v<T>
	static constexpr This _s_fnv1a_hash(Span<T> span) {
		if constexpr (Type_IsSame<VALUE, u32>) {
			u32 hash = 0x811C9DC5u;
			for (auto& s : span) {
				hash ^= static_cast<u32>(s);
				hash *= 0x01000193u;
			}
			return hash;
		
		} else if constexpr (Type_IsSame<VALUE, u64>) {
			u64 hash = 0xCBF29CE484222325ull;
			for (auto& s : span) {
				hash ^= static_cast<u64>(s);
				hash *= 0x100000001B3ull;
			}
			return hash;
		} else {
			static_assert(false);
		}
	}	
};

using HashInt32 = HashInt_<u32>;
using HashInt64 = HashInt_<u64>;
using HashInt   = HashInt64;

template<class OBJ> struct HashInt_Handler;

template<class OBJ> AX_INLINE constexpr
HashInt HashInt_get(const OBJ& v) { return HashInt_Handler<OBJ>().onHashInt(v); }

template<class OBJ>
struct HashInt_Handler {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& obj) { return obj.onHashInt(); }
};

template<class A, class B>
struct HashInt_Handler< Pair<A,B> > {
	AX_INLINE constexpr static HashInt onHashInt(const Pair<A,B>& obj) {
		return HashInt_get(obj.first) ^ HashInt_get(obj.second);
	}
};

template<class OBJ> requires std::is_enum_v<OBJ>
struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) { return HashInt::s_make(static_cast<u64>(v)); }
};

template<class OBJ> requires std::is_fundamental_v<OBJ>
struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) { return HashInt::s_make(static_cast<u64>(v)); }
};

template <class OBJ>
requires std::is_convertible_v<OBJ, StrViewA>
      || std::is_convertible_v<OBJ, StrViewW>
      || std::is_convertible_v<OBJ, StrView8>
      || std::is_convertible_v<OBJ, StrView16>
      || std::is_convertible_v<OBJ, StrView32>

struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) {
		       if constexpr (std::is_convertible_v<OBJ, StrViewA >) { return HashInt::s_fnv1a_hash(StrViewA( v).toByteSpan());
		} else if constexpr (std::is_convertible_v<OBJ, StrViewW >) { return HashInt::s_fnv1a_hash(StrViewW( v).toByteSpan());
		} else if constexpr (std::is_convertible_v<OBJ, StrView8 >) { return HashInt::s_fnv1a_hash(StrView8( v).toByteSpan());
		} else if constexpr (std::is_convertible_v<OBJ, StrView16>) { return HashInt::s_fnv1a_hash(StrView16(v).toByteSpan());
		} else if constexpr (std::is_convertible_v<OBJ, StrView32>) { return HashInt::s_fnv1a_hash(StrView32(v).toByteSpan());
		} else { static_assert(false); }
		
	}
};

template <class OBJ> requires std::is_convertible_v<OBJ, StrViewW>
struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) { return HashInt::s_fnv1a_hash(StrViewW(v).span()); }
};

template <class OBJ> requires std::is_convertible_v<OBJ, StrView8>
struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) { return HashInt::s_fnv1a_hash(StrView8(v).span()); }
};

template <class OBJ> requires std::is_convertible_v<OBJ, StrView16>
struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) { return HashInt::s_fnv1a_hash(StrView16(v).span()); }
};

template <class OBJ> requires std::is_convertible_v<OBJ, StrView32>
struct HashInt_Handler<OBJ> {
	AX_INLINE constexpr static HashInt onHashInt(const OBJ& v) { return HashInt::s_fnv1a_hash(StrView32(v).span()); }
};

} // namespace