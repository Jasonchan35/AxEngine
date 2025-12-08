module;
#include "AxCore-pch.h"

export module AxCore.JsonWriter;
export import AxCore.Logger;
export import AxCore.Float16;
export import AxCore.JsonValue;

export namespace ax {

class JsonWriter : public NonCopyable {
public:
	JsonWriter(IString & outJson);
	~JsonWriter();

	class ObjectScope : public NonCopyable {
	public:
		ObjectScope(JsonWriter* p);
		ObjectScope(ObjectScope && rhs) noexcept;
		~ObjectScope();

	private:
		JsonWriter* _p;
	};

	class ArrayScope : public NonCopyable {
	public:
		ArrayScope(JsonWriter* p);
		ArrayScope(ArrayScope && rhs) noexcept;
		~ArrayScope();
	private:
		JsonWriter* _p;
	};

	AX_NODISCARD ObjectScope objectScope(StrView name)		{ beginObject(name); return ObjectScope(this); }
	AX_NODISCARD ObjectScope objectScope()					{ beginObject();	 return ObjectScope(this); }

	template<class FUNC> void writeObject(FUNC func)				{ auto scope = objectScope();		func(); }
	template<class FUNC> void writeObject(StrView name, FUNC func)	{ auto scope = objectScope(name);	func(); }

	ArrayScope	arrayScope(StrView name)			{ beginArray(name); return ArrayScope(this); }
	ArrayScope	arrayScope()						{ beginArray();		return ArrayScope(this); }

	template<class FUNC>	void writeArray(FUNC func)					{ auto scope = arrayScope(); func(); }
	template<class FUNC>	void writeArray(StrView name, FUNC func)	{ auto scope = arrayScope(name); func(); }

	void beginObject(StrView name);
	void beginObject();
	void endObject();

	void beginArray(StrView name);
	void beginArray();
	void endArray();

	void writeMemberName(StrView name);

	template<class VALUE>
	void writeMember(StrView name, const VALUE& value) {
		writeMemberName(name);
		writeValue(value);
	}

	void writeNull();

	void writeValue(i8		value) { writeValue(static_cast<i64>(value)); }
	void writeValue(i16		value) { writeValue(static_cast<i64>(value)); }
	void writeValue(i32		value) { writeValue(static_cast<i64>(value)); }
	void writeValue(i64		value);

	void writeValue(u8		value) { writeValue(static_cast<u64>(value)); }
	void writeValue(u16		value) { writeValue(static_cast<u64>(value)); }
	void writeValue(u32		value) { writeValue(static_cast<u64>(value)); }
	void writeValue(u64		value);

	void writeValue(f16		value) { writeValue(value.to_f64()); }
	void writeValue(f32		value) { writeValue(static_cast<f64>(value)); }
	void writeValue(f64		value);

	void writeValue(std::nullptr_t)		{ writeNull(); }
	void writeValue(bool		value);

	void writeValue(StrViewA	value);
	void writeValue(StrViewW	value);
	void writeValue(StrView16	value);
	void writeValue(StrView32	value);

	void writeValue(const JsonValue&	value);
	void writeValue(const JsonObject&	value);
	void writeValue(const JsonArray&	value);

	template<Int N>	void writeValue(const CharA  (&sz)[N])	{ writeValue(StrView(sz)); }
	template<Int N>	void writeValue(const CharW  (&sz)[N])	{ writeValue(StrView(sz)); }
	template<Int N>	void writeValue(const Char16 (&sz)[N])	{ writeValue(StrView(sz)); }
	template<Int N>	void writeValue(const Char32 (&sz)[N])	{ writeValue(StrView(sz)); }

	template<class R>
	void writeValue(const IString_<R>& value) { writeValue(value.view()); }

	template<class R, Int N>
	void writeValue(const String_<R, N>& value) { writeValue(value.view()); }

	IString& json() { return *_json; }

	void preWriteValue();
	void newline(Int offset = 0);

private:
	void _writeQuoteString(StrViewA  v);
	void _writeQuoteString(StrViewW  v);
	void _writeQuoteString(StrView16 v);
	void _writeQuoteString(StrView32 v);

	void _writeEncodeStringChar(CharA ch);

	template<class R>
	void _writeQuoteUtfString(StrView_<R> v);

	enum class LevelType {
		Object,
		Array,
		Member,
	};

	IStringA*	_json;
	Array<LevelType, 16> _level;

	bool _commaNeeded	{false};
	bool _newlineNeeded {true};
};

AX_INLINE
void JsonWriter::_writeEncodeStringChar(CharA ch) {
	switch (ch) {
		case '/':	AX_FALLTHROUGH
		case '\\':	AX_FALLTHROUGH
		case '\"':	*_json << '\\' << ch;	break;
		case '\b':	*_json << "\\b";		break;
		case '\f':	*_json << "\\f";		break;
		case '\n':	*_json << "\\n";		break;
		case '\r':	*_json << "\\r";		break;
		case '\t':	*_json << "\\t";		break;
		default:	*_json << ch;			break;
	}
}

} // namespace