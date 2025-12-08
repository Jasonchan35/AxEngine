module;
#include "AxCore-pch.h"

export module AxCore.JsonIO_Writer;
export import AxCore.JsonIO_Reader;
export import AxCore.JsonWriter;

export namespace ax {


class JsonIO_Writer : public NonCopyable {
public:
	static constexpr bool isReader() { return false; }
	static constexpr bool isWriter() { return true;  }

	JsonIO_Writer(IString& outJson) : writer(outJson) {}

	void io(bool&		value) { writer.writeValue(value); }

	void io(i8&			value) { writer.writeValue(value); }
	void io(i16&		value) { writer.writeValue(value); }
	void io(i32&		value) { writer.writeValue(value); }
	void io(i64&		value) { writer.writeValue(value); }

	void io(u8&			value) { writer.writeValue(value); }
	void io(u16&		value) { writer.writeValue(value); }
	void io(u32&		value) { writer.writeValue(value); }
	void io(u64&		value) { writer.writeValue(value); }

	void io(f16&		value) { writer.writeValue(value); }
	void io(f32&		value) { writer.writeValue(value); }
	void io(f64&		value) { writer.writeValue(value); }

	void io(CharA&		value) { writer.writeValue(StrView_char(value)); }
	void io(CharW&		value) { writer.writeValue(StrView_char(value)); }
	void io(Char16&		value) { writer.writeValue(StrView_char(value)); }
	void io(Char32&		value) { writer.writeValue(StrView_char(value)); }

	void io(JsonValue&	value) { writer.writeValue(value); }

	template<class T, Int N> AX_INLINE void io_fixed_span(MutFixedSpan<T, N> value);

//	template<class T> AX_INLINE void io_span(Span<T> value);
	template<class T> AX_INLINE void io_array(IArray<T>& value);
	template<class T> AX_INLINE void io_string(StrView_<T>& value) { writer.writeValue(value); }
	template<class T> AX_INLINE void io_string(IString_<T>& value) { writer.writeValue(value); }

	template<class T> AX_INLINE
	void io(T& value) { JsonIO_Handler<T>::onJsonIO(*this, value); }

	template<class T> AX_INLINE
	void named_io(StrView name, T& value) {
		writer.writeMemberName(name);
		io(value);
	}

	template<class T> void named_ioEnumAsInt(StrView name, T& value) {
		static_assert(std::is_enum_v<T>);
		auto tmp = ax_enum_int(value);
		named_io(name, tmp);
	}

	template<class T>
	struct ReflectionHandler {
		template<Int index, class FIELD>
		static void onEach(JsonIO_Writer& se, T& obj) {
			se.named_io(FIELD::s_name(), FIELD::s_value(&obj));
		}
	};

	template<class T>
	void ownFields_io(T& obj) {
		MetaTypeOf<T>::OwnFields::template s_forEachType<ReflectionHandler<T>>(*this, obj);
	}

	template<class T>
	void allFields_io(T& obj) {
		MetaTypeOf<T>::OwnFields::template s_forEachType<ReflectionHandler<T>>(*this, obj);
	}

	JsonWriter writer;
};

template<class T> inline
void ax_dump_json_impl(const SrcLoc& loc, T& value) {
	TempStringA json;
	JsonIO_Writer se(json);
	se.io(value);
	AX_LOG("AX_DUMP_JSON: {}", json);
}

#define AX_DUMP_JSON(value) ax_dump_json_impl(AX_SRC_LOC, value);

template<class T, Int N> inline
void JsonIO_Writer::io_fixed_span(MutFixedSpan<T, N> value) {
	auto array_scope = writer.arrayScope();
	for (auto& e : value) {
		io(e);
	}
}

//template<class T> inline
//void JsonIO_Writer::io_span(Span<T> value) {
//	auto array_scope = writer.arrayScope();
//	for (auto& e : value) {
//		io(e);
//	}
//}

template<class T> inline
void JsonIO_Writer::io_array(IArray<T>& value) {
	auto array_scope = writer.arrayScope();
	for (auto& e : value) {
		io(e);
	}
}

template<class T>
struct JsonIO_Writer_Handler {
	static void onJsonIO(JsonIO_Writer& se, T& value) {
		if constexpr (CON_onJsonIO_Value<T, JsonIO_Writer>) {
			value.onJsonIO_Value(se);
		} else {
			se.writer.writeObject([&](){
				value.onJsonIO(se);
			});
		}
	}
};

} // namespace
