module;
#include "AxCore-pch.h"

export module AxCore.Uuid;
export import AxCore.String;

#if !AX_USE_PRECOMPILE_HEADER && AX_OS_WINDOWS
export import <rpc.h>; // uuid
#pragma comment( lib, "Rpcrt4.lib" )
#endif

export namespace ax {

AX_SIMPLE_ERROR(Error_Uuid)

class Uuid {
public:
	static constexpr Int kDataSize = 16;

	using Value = Pair<u64, u64>;

	Value value;

	static constexpr Value	kZeroValue() { return Value(0,0); }

	struct HumanRead {
		u32 e0;
		u16 e1;
		u16 e2;
		u64 e3;
	};

	Uuid() = default;

			HumanRead&	humanRead() 		{ return *reinterpret_cast<      HumanRead*>(this); }
	const 	HumanRead&	humanRead() const 	{ return *reinterpret_cast<const HumanRead*>(this); }

	MutByteSpan		toByteSpan()		{ return MutByteSpan(reinterpret_cast<      Byte*>(&value), kDataSize); }
	   ByteSpan		toByteSpan() const	{ return    ByteSpan(reinterpret_cast<const Byte*>(&value), kDataSize); }

	AX_NODISCARD static Uuid s_gen() { return Uuid(Tag_Gen()); }

	AX_NODISCARD bool operator==(const Uuid& r) const { return value == r.value; }
	AX_NODISCARD bool operator!=(const Uuid& r) const { return value != r.value; }
	AX_NODISCARD bool operator< (const Uuid& r) const { return value <  r.value;  }
	AX_NODISCARD bool operator<=(const Uuid& r) const { return value <= r.value;  }
	AX_NODISCARD bool operator> (const Uuid& r) const { return value >  r.value;  }
	AX_NODISCARD bool operator>=(const Uuid& r) const { return value >= r.value;  }

	AX_NODISCARD constexpr bool isValid() const { return value != kZeroValue(); }
	AX_NODISCARD constexpr explicit operator bool() const { return isValid(); }

	AX_NODISCARD AX_INLINE Uuid swapByteOrderForVariant2() const;

	bool tryParse(StrView str);
	void parse(StrView str) { if (!tryParse(str)) throw Error_ParseString(); }

	void getString(IString& o) const;

	String	toString() const { String s; getString(s); return s; }

	HashInt	onHashInt() const { return HashInt::s_make(value); }

	template<class CH> void onFmt(Format_<CH>& ctx) const { ctx << toString(); }

private:
	enum class Tag_Gen {};
	Uuid(Tag_Gen);
};

static_assert(sizeof(Uuid) == Uuid::kDataSize);

// Variant 2 UUIDs, historically used in Microsoft's COM/OLE libraries, use a mixed-endian format, 
// whereby the first three components of the UUID are little-endian, and the last two are big-endian. 
// For example, 00112233-4455-6677-8899-aabbccddeeff is encoded 
// as the bytes 33 22 11 00 55 44 77 66 88 99 aa bb cc dd ee ff.

inline
Uuid Uuid::swapByteOrderForVariant2() const {
	Uuid o;
	auto& src = humanRead();
	auto& dst = o.humanRead();	
	using S = ByteOrder::ByteSwapUInt;
	dst.e0 = S::convert(src.e0);
	dst.e1 = S::convert(src.e1);
	dst.e2 = S::convert(src.e2);
	dst.e3 = src.e3;
	return o;
}

inline
void Uuid::getString(IString& o) const {
	o.clear();
	auto data = toByteSpan();
	for (Int i = 0; i < kDataSize; i++) {
		if (i == 4 || i == 6 || i == 8 || i == 10) {
			o << "-";
		}

		auto hex = CharUtil::byteToHex<Char>(data[i]);
		o << hex;
	}
}

inline
bool Uuid::tryParse(StrView str) {
	static constexpr Int N = 36;

	if (str.size() < N) return false;

	auto data = toByteSpan();

	Int t = 0;
	for (Int i = 0; i < N; i++) {
		auto ch = str[i];

		if (i == 8 || i == 13 || i == 18 || i == 23) {
			if (ch != '-') return false;
			continue;
		}

		auto v = CharUtil::hexToByte(ch);
		if (!v) return false;
		
		if (t % 2 == 0) {
			data[t/2] = static_cast<Byte>(v.value() << 4);
		} else {
			data[t/2] = static_cast<Byte>(v.value() | data[t/2]);
		}
		t++;
	}

	return true;
}

#if AX_OS_WINDOWS

Uuid::Uuid(Tag_Gen) {
	auto span = toByteSpan();

	static_assert(sizeof(UUID) == kDataSize);
	RPC_STATUS ret = ::UuidCreate(reinterpret_cast<UUID*>(span.data()));
	if (ret != RPC_S_OK) throw Error_Uuid();
}

#else //=====================================

Uuid::Uuid(Tag_Gen) {
	auto span = toByteSpan();
	uuid_generate(span.data());
}

#endif

} // namespace