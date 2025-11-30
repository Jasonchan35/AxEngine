module;
#include "AxCore-pch.h"
export module AxCore.HashInt;
export import AxCore.BasicType;
export import AxCore.Span;

export namespace ax {

template<class T> struct HashInt_Handler;

class HashInt {
	using This = HashInt;
	using VALUE = u64;
	constexpr HashInt(const VALUE& v) : value(v) {} // private
public:
	using Value = VALUE;
	Value	value;

	template<class OBJ>
	static constexpr This s_get(const OBJ& obj);

	constexpr This operator^(const This& other) const { return value ^ other.value; }

	static constexpr This s_fromInt(const Value& v) { return This(v); }

	
	template<class T> requires std::is_integral_v<T>
	static constexpr This s_fnv1a_hash(Span<T> span) {
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

template<class T> requires std::is_integral_v<T>
struct HashInt_Handler<Span<T>> {
	AX_INLINE constexpr static HashInt onHashInt(const Span<T>& obj) {
		return HashInt::s_fnv1a_hash(obj);
	}
};

template<class A, class B>
struct HashInt_Handler<Pair<A,B> > {
	AX_INLINE constexpr static HashInt onHashInt(const Pair<A,B>& obj) {
		return HashInt::s_get(obj.first) ^ HashInt::s_get(obj.second);
	}
};

template<class T> requires std::is_enum_v<T>
struct HashInt_Handler<T> {
	AX_INLINE constexpr static HashInt onHashInt(const T& v) { return HashInt::s_get(static_cast<HashInt::Value>(v)); }
};

template<class T> requires std::is_fundamental_v<T>
struct HashInt_Handler<T> {
	AX_INLINE constexpr static HashInt onHashInt(const T& v) { return HashInt::s_get(static_cast<HashInt::Value>(v)); }
};

template<class T>
struct HashInt_Handler {
	AX_INLINE constexpr static HashInt onHashInt(const T& obj) { return obj.onHashInt(); }
};

template<class OBJ> inline
constexpr HashInt HashInt::s_get(const OBJ& obj) { return HashInt_Handler<OBJ>::onHashInt(obj); }

} // namespace