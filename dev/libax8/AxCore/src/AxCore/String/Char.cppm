module;

export module AxCore.CharUtil;
export import AxCore.BasicType;

export namespace ax {

template<Int N>	struct CharW_Native_;
template<>	struct CharW_Native_<AX_SIZEOF(Char16)> { using Type = Char16; };
template<>	struct CharW_Native_<AX_SIZEOF(Char32)> { using Type = Char32; };
using CharW_Native = typename CharW_Native_<AX_SIZEOF(CharW)>::Type;

AX_INLINE			CharW_Native	CharW_toNative(      CharW   v) { return static_cast<           CharW_Native  >(v); }
AX_INLINE			CharW_Native&	CharW_toNative(      CharW&  v) { return reinterpret_cast<      CharW_Native& >(v); }
AX_INLINE			CharW_Native*	CharW_toNative(      CharW*  v) { return reinterpret_cast<      CharW_Native* >(v); }
AX_INLINE			CharW_Native**	CharW_toNative(      CharW** v) { return reinterpret_cast<      CharW_Native**>(v); }
AX_INLINE	const	CharW_Native&	CharW_toNative(const CharW&  v) { return reinterpret_cast<const CharW_Native& >(v); }
AX_INLINE	const	CharW_Native*	CharW_toNative(const CharW*  v) { return reinterpret_cast<const CharW_Native* >(v); }
AX_INLINE	const	CharW_Native**	CharW_toNative(const CharW** v) { return reinterpret_cast<const CharW_Native**>(v); }


struct CharUtil {
	CharUtil() = delete;
	template<class CH> AX_NODISCARD AX_INLINE static constexpr bool isAlpha	(CH ch) { return std::isalpha(ch); } 
	template<class CH> AX_NODISCARD AX_INLINE static constexpr bool isDigit	(CH ch) { return std::isdigit(ch); }
	template<class CH> AX_NODISCARD AX_INLINE static constexpr bool isUpper	(CH ch) { return std::isupper(ch); }
	template<class CH> AX_NODISCARD AX_INLINE static constexpr bool isLower	(CH ch) { return std::islower(ch); }
	template<class CH> AX_NODISCARD AX_INLINE static constexpr CH   toUpper	(CH ch) { return static_cast<CH>(std::toupper(ch)); }
	template<class CH> AX_NODISCARD AX_INLINE static constexpr CH   toLower	(CH ch) { return static_cast<CH>(std::tolower(ch)); }

	template <class CH>
	AX_NODISCARD AX_INLINE static constexpr bool equals(CH a, CH b, StrCase sc) { return compare(a, b, sc) == CmpResult::Equal; }
	template <class CH>
	AX_NODISCARD AX_INLINE static constexpr bool equals_i(CH a, CH b) { return compare_<StrCase::Ignore>(a, b) == CmpResult::Equal; }
	template <StrCase SC, class CH>
	AX_NODISCARD AX_INLINE static constexpr bool equals_(CH a, CH b) { return compare_<SC>(a, b) == CmpResult::Equal; }
	template <class CH>
	AX_NODISCARD AX_INLINE static constexpr CmpResult compare_i(CH a, CH b) { return compare_<StrCase::Ignore>(a, b); }

	template <class CH>
	AX_NODISCARD AX_INLINE static constexpr CmpResult compare(CH a, CH b, StrCase sc) {
		return sc == StrCase::Ignore ? compare_<StrCase::Ignore>(a, b) : compare_<StrCase::Sensitive>(a, b);
	}

	template <StrCase SC, class CH>
	AX_NODISCARD AX_INLINE static constexpr CmpResult compare_(CH a, CH b) {
		if constexpr (SC == StrCase::Ignore) {
			auto diff = toLower(a) - toLower(b);
			return CmpResult_fromInt(diff);
		} else {
			auto diff = a - b;
			return CmpResult_fromInt(diff);
		}
	}
	
	template<class CH>
	AX_NODISCARD AX_INLINE static constexpr bool isHex(CH ch) {
		if( ch >= '0' && ch <='9' ) return true;
		if( ch >= 'A' && ch <='F' ) return true;
		if( ch >= 'a' && ch <='f' ) return true;
		return false;
	}

	template<class CH>
	AX_NODISCARD AX_INLINE static constexpr Opt<u8> hexToByte(CH ch) {
		if( ch >= '0' && ch <= '9' ) return static_cast<u8>(ch - '0');
		if( ch >= 'a' && ch <= 'f' ) return static_cast<u8>(ch - 'a' + 10);
		if( ch >= 'A' && ch <= 'F' ) return static_cast<u8>(ch - 'A' + 10);
		return std::nullopt;
	}

	template<class CH>
	AX_NODISCARD AX_INLINE static constexpr Opt<u8> hexToByte(CH c0, CH c1) {
		auto v0 = hexToByte(c0);
		auto v1 = hexToByte(c1);
		if (!v0 || !v1) return std::nullopt;
		return (v0.value() << 4) | v1.value();
	}
	
	template<class CH>
	AX_NODISCARD AX_INLINE static constexpr CharHexPair<CH> byteToHex(u8 ch) {
		constexpr char hex[] = "0123456789ABCDEF";
		return CharHexPair(	static_cast<CH>(hex[(ch >> 4) & 0xF]),
							static_cast<CH>(hex[ ch       & 0xF]));
	}

	static constexpr char32_t FourCC(char a, char b, char c, char d ) {
		#if AX_CPU_ENDIAN_LITTLE
			return static_cast<char32_t>(a)
				 | static_cast<char32_t>(b) << 8
				 | static_cast<char32_t>(c) << 16
				 | static_cast<char32_t>(d) << 24;
		#elif AX_CPU_ENDIAN_BIG
			return static_cast<char32_t>(a) << 24
				 | static_cast<char32_t>(b) << 16
				 | static_cast<char32_t>(c) << 8
				 | static_cast<char32_t>(d);
		#else
			#error "Unknown Host Endian"
		#endif
	}

	static constexpr char32_t FourCC(const char (&c)[5]) { return FourCC(c[0], c[1], c[2], c[3]); }

	template<class CH>
	static constexpr CH translateEscapeSequence(CH c) {
		switch (c) { // https://en.cppreference.com/w/cpp/language/escape
			case 'a':	return CH('\a');
			case 'b':	return CH('\b');
			case 'f':	return CH('\f');
			case 'n':	return CH('\n');
			case 'r':	return CH('\r');
			case 't':	return CH('\t');
			case 'v':	return CH('\v');
			default:	return c;
		}
	}
};



} // namespace
