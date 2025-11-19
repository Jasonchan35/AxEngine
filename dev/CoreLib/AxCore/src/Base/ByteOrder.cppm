export module AxCore.ByteOrder;

#include "AxBase.h"
import AxCore.BasicType;

export namespace ax {

struct ByteOrder {
	struct StraightUInt {
		constexpr static u8  convert(u8  v) { return v; }
		constexpr static u16 convert(u16 v) { return v; }
		constexpr static u32 convert(u32 v) { return v; }
		constexpr static u64 convert(u64 v) { return v; }
	};

	class ByteSwapUInt {
	public:
		constexpr static u8  convert(u8  v) {
			return v; 
		}
		constexpr static u16 convert(u16 v) {
			return (u16)((v>>8) | (v<<8));
		}
		constexpr static u32 convert(u32 v) {
			v = ((v<<8) & 0xFF00FF00UL) | ((v>>8) & 0x00FF00FFUL); 
			return (v>>16) | (v<<16);
		}
		constexpr static u64 convert(u64 v) { 
			v = ((v<< 8) & 0xFF00FF00FF00FF00ULL) | ((v>> 8) & 0x00FF00FF00FF00FFULL);
			v = ((v<<16) & 0xFFFF0000FFFF0000ULL) | ((v>>16) & 0x0000FFFF0000FFFFULL);
			return (v>>32) | (v<<32);
		}
	};

	template <class ToHost, class FromHost>
	class Convertor {
	public:
		//to host
		constexpr static u8  toHost(u8  v) { return ToHost::convert(v); }
		constexpr static u16 toHost(u16 v) { return ToHost::convert(v); }
		constexpr static u32 toHost(u32 v) { return ToHost::convert(v); }
		constexpr static u64 toHost(u64 v) { return ToHost::convert(v); }
		constexpr static i8  toHost(i8  v) { return static_cast<i8 >(ToHost::convert(static_cast<u8 >(v))); }
		constexpr static i16 toHost(i16 v) { return static_cast<i16>(ToHost::convert(static_cast<u16>(v))); }
		constexpr static i32 toHost(i32 v) { return static_cast<i32>(ToHost::convert(static_cast<u32>(v))); }
		constexpr static i64 toHost(i64 v) { return static_cast<i64>(ToHost::convert(static_cast<u64>(v))); }

		AX_INLINE static f32 toHost(f32 v) {
			u32 i = ToHost::convert(*reinterpret_cast<u32*>(&v));
			return *reinterpret_cast<f32*>(&i);
		}

		AX_INLINE static f64 toHost(f64 v) {
			u64 i = ToHost::convert(*reinterpret_cast<u64*>(&v));
			return *reinterpret_cast<f64*>(&i);
		}

		// from host
		constexpr static u8  fromHost(u8  v) { return FromHost::convert(v); }
		constexpr static u16 fromHost(u16 v) { return FromHost::convert(v); }
		constexpr static u32 fromHost(u32 v) { return FromHost::convert(v); }
		constexpr static u64 fromHost(u64 v) { return FromHost::convert(v); }
		constexpr static i8  fromHost(i8  v) { return static_cast<i8 >(FromHost::convert(static_cast<u8 >(v))); }
		constexpr static i16 fromHost(i16 v) { return static_cast<i16>(FromHost::convert(static_cast<u16>(v))); }
		constexpr static i32 fromHost(i32 v) { return static_cast<i32>(FromHost::convert(static_cast<u32>(v))); }
		constexpr static i64 fromHost(i64 v) { return static_cast<i64>(FromHost::convert(static_cast<u64>(v))); }

		constexpr static f32 fromHost(f32 v) {
			u32 i = FromHost::convert(*reinterpret_cast<u32*>(&v));
			return *reinterpret_cast<f32*>(&i);
		}

		constexpr static f64 fromHost(f64 v) {
			u64 i = FromHost::convert(*reinterpret_cast<u64*>(&v));
			return *reinterpret_cast<f64*>(&i);
		}
	};

	using StraightConvertor = Convertor<StraightUInt, StraightUInt>;
	using ByteSwapConvertor = Convertor<ByteSwapUInt, ByteSwapUInt>;

#if AX_CPU_ENDIAN_LITTLE
	using LittleEndian = StraightConvertor;
	using BigEndian    = ByteSwapConvertor;
	using HostEndian   = LittleEndian;

#elif AX_CPU_ENDIAN_BIG
	using LittleEndian = ByteSwapConvertor;
	using BigEndian    = StraightConvertor;
	using HostEndian   = BigEndian;

#else
	#error "Unknown Host Endian"
#endif

};

} // namespace
