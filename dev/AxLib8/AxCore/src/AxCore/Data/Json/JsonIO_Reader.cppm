module;


export module AxCore.JsonIO_Reader;
export import AxCore.JsonReader;
export import AxCore.Rtti;

export namespace ax {

template<class OBJ, class SE>
concept CON_onJsonIO_Value = requires(OBJ& obj, SE & se) {
	{ obj.onJsonIO_Value(se) } -> std::same_as<void>;
};

template<class OBJ, class SE>
constexpr bool Type_onJsonIO_Value = CON_onJsonIO_Value<OBJ, SE>;

template<class T, class ENABLE_IF = void>
struct JsonIO_Handler {
	template<class SE>
	static void onJsonIO(SE& se, T& value) {
		static_assert(!Type_IsSpan<T>,     "please use FixedSpan / IArray for IO, since Span cannot do dynamic resize");
		static_assert(!Type_IsStrView<T>,  "please use IString for IO, since StrView cannot do dynamic resize");

		if constexpr (Type_onJsonIO_Value<T, SE>) {
			value.onJsonIO_Value(se);
		} else {
			if constexpr (se.isReader()) {
				se.reader.readObject([&]() { value.onJsonIO(se); });
			} else {
				se.writer.writeObject([&]() { value.onJsonIO(se); });
			}
		}
	}
};

class JsonIO_Reader : public NonCopyable {
public:
	static constexpr bool isReader() { return true; }
	static constexpr bool isWriter() { return false;  }

	JsonIO_Reader(StrViewA json, StrView filename)
		: reader(json, filename) 
	{
	}

	void io(bool&		value) { reader.readValue(value); }

	void io(i8&			value) { reader.readValue(value); }
	void io(i16&		value) { reader.readValue(value); }
	void io(i32&		value) { reader.readValue(value); }
	void io(i64&		value) { reader.readValue(value); }

	void io(u8&			value) { reader.readValue(value); }
	void io(u16&		value) { reader.readValue(value); }
	void io(u32&		value) { reader.readValue(value); }
	void io(u64&		value) { reader.readValue(value); }

	void io(f16&		value) { reader.readValue(value); }
	void io(f32&		value) { reader.readValue(value); }
	void io(f64&		value) { reader.readValue(value); }

	void io(JsonValue&	value) { reader.readValue(value); }

	template<class T>
	void io(T& value) { JsonIO_Handler<T>::onJsonIO(*this, value); }

	template<class T>
	bool member_io(StrView name, T& value) {
		if (!reader.isMember(name)) return false;
		io(value);
		return true;
	}

	template<class T> void member_ioEnumAsInt(StrView name, T& value) {
		static_assert(std::is_enum_v<T>);
		auto tmp = ax_enum_int(value);
		member_io(name, tmp);
		ax_enum_set_int(value, tmp);
	}

	template<class T> void member_io_fixed(StrView name, T& value) { member_io(name, value); }

	template<class T, Int N> void io_fixed_span(MutFixedSpan<T, N> value);
//	template<class T> void io_span(MutSpan<T> value);
	template<class T> void io_array(IArray<T>&  value);
	template<class T> void io_string(IString_<T>& value) { reader.readValue(value); }

	template<class T>
	struct ReflectionHandler {
		template<Int index, class FIELD>
		static void onEach(JsonIO_Reader& se, T& obj) {
			se.member_io(FIELD::s_name(), FIELD::s_value(&obj));
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

	JsonReader reader;
};

template<class T, Int N> inline
void JsonIO_Reader::io_fixed_span(MutFixedSpan<T, N> value) {
	Int i = 0;
	reader.readArray([&]() {
		if (i >= value.size()) {
			reader.warning("too many span elements");
			return;
		}
		io(value[i]);
		i++;
	});

	if (i != value.size()) {
		reader.warning("span size mismatch");
	}
}

//template<class T> inline
//void JsonIO_Reader::io_span(MutSpan<T> value) {
//	Int i = 0;
//	reader.readArray([&]() {
//		if (i >= value.size()) {
//			reader.warning("too many span elements");
//			return;
//		}
//		io(value[i]);
//		i++;
//	});
//
//	if (i != value.size()) {
//		reader.warning("span size mismatch");
//	}
//}

template<class T> inline
void JsonIO_Reader::io_array(IArray<T>& value) {
	value.clear();
	reader.readArray([&]() {
		io(value.emplaceBack());
	});
};

} // namespace