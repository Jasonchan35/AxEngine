module;
#include "AxCore-pch.h"

export module AxCore.JsonReader;
export import AxCore.Logger;
export import AxCore.Float16;
export import AxCore.JsonValue;
export import AxCore.LexerSource;

export namespace ax {


class JsonReader : public NonCopyable {
public:
	enum class ValueType {
		Invalid,
		BeginObject,
		EndObject,
		BeginArray,
		EndArray,
		f64,
		i64,
		u64,
		Bool,
		Null,
		String,
		Member,
	};

	JsonReader(StrView json, StrView filenameForErrorMessage);

	void readJson(StrView json, StrView filename);

	ValueType valueType() { return _valueType; }

	bool valueIsString	() const	{ return _valueType == ValueType::String; }
	bool valueIsNumber	() const	{ return valueIsInt64() || valueIsUInt64() || valueIsDouble(); }

	bool valueIsUInt64	() const	{ return _valueType == ValueType::u64; }
	bool valueIsInt64	() const	{ return _valueType == ValueType::i64; }
	bool valueIsDouble	() const	{ return _valueType == ValueType::f64; }

	bool valueIsBool	() const	{ return _valueType == ValueType::Bool; }
	bool valueIsObject	() const	{ return _valueType == ValueType::BeginObject; }
	bool valueIsArray	() const	{ return _valueType == ValueType::BeginArray; }
	bool valueIsNull	() const	{ return _valueType == ValueType::Null; }

	StrView valueStrView() const	{ return valueIsString() ? _token.str.view() : StrView(); }

	bool beginObject(StrView name);
	void beginObject();
	bool isEndObject();
	bool endObject();
	template<class FUNC> void readObject(FUNC func);

	bool beginArray(StrView name);
	void beginArray();
	bool isEndArray();
	bool endArray();
	bool expectEndArray();
	template<class FUNC> void readArray(FUNC func);


	bool isMember(StrView name);
	StrView isMemberHasPrefix(StrView prefix);

	template<class T>
	bool readMember(StrView name, T& outValue);
	template<class FUNC> bool readMemberArray(StrView name, FUNC func);
	template<class FUNC> bool readMemberObject(StrView name, FUNC func);

	void readMemberName(IString& outValue);
	bool peekMemberName(IString& outValue);

	void readValue(bool&	outValue);

	void readValue(i8&		outValue);
	void readValue(i16&		outValue);
	void readValue(i32&		outValue);
	void readValue(i64&		outValue);

	void readValue(u8&		outValue);
	void readValue(u16&		outValue);
	void readValue(u32&		outValue);
	void readValue(u64&		outValue);

	void readValue(f16&		outValue);
	void readValue(f32&		outValue);
	void readValue(f64&		outValue);

	void readValue(JsonValue&	outValue);
	void readValue(JsonObject&	outValue);
	void readValue(JsonArray&	outValue);

	template<class R>
	void readValue(IString_<R>&	outValue);

	template<class R, Int N>
	void readValue(String_<R, N>&	outValue) { readValue(outValue.asIString()); }

	template<class R>
	void readValue(MutSpan<R> outValue);

	template<class R>
	void readValue(IArray<R>& outValue);

	void readNullValue();

	void skipValue();
	void skipMemberName();

	void reset();
	bool next();

	template<class... ARGS> void info	(StrView fmtStr, ARGS&&... args) { log(LogLevel::Info,		fmtStr, AX_FORWARD(args)...); }
	template<class... ARGS> void warning(StrView fmtStr, ARGS&&... args) { log(LogLevel::Warning,	fmtStr, AX_FORWARD(args)...); }
	template<class... ARGS> void error	(StrView fmtStr, ARGS&&... args) { log(LogLevel::Error,		fmtStr, AX_FORWARD(args)...); }

	template<class... ARGS>
	void log(LogLevel lv, StrView fmtStr, ARGS&&... args);

	void dumpToken();
	void dumpValue();

	using Source = LexerSource<char>;

	const Source&	source() const { return _source; }

	bool unhandledMember();
	bool skipUnhandledMember(bool showWarning);
	bool onUnhandledMemeber();

private:
	template<class V> void _readNumericValue(V&	outValue);

	enum class TokenType {
		Invalid,
		BeginObject,
		EndObject,
		BeginArray,
		EndArray,
		f64,
		u64,
		i64,
		Identifier,
		Bool,
		Null,
		String,
		Comma,
		Colon,
	};

	bool _nextToken();
	void _nextChar() { _ch = _source.nextChar(); }
	void _parseStringToken();

	LexerSource<char>	_source;
	char _ch = 0;

	struct Token {
		TokenType	type = TokenType::Invalid;
		TempString	str;
		bool		boolValue = false;
	};

	Token		_token;
	ValueType	_valueType = ValueType::Invalid;

	struct Level {
		enum class Type {
			None,
			Object,
			Array,
		};

		Level(Type type_ = Type::None, Int pos_ = 0) : type(type_), pos(pos_) {}

		Type type = Type::None;
		Int	 pos  = 0;
	};

	Level::Type _currentLevelType() {
		return _levels.size() ? _levels.back().type : Level::Type::None;
	}

	Array<Level, 32>	_levels;

	StrViewA	_json;
};

template<class T> inline
bool JsonReader::readMember(StrView name, T& outValue) {
	if (!isMember(name)) return false;
	readValue(outValue);
	return true;
}

template<class FUNC> inline
bool JsonReader::readMemberObject(StrView name, FUNC func) {
	if (!isMember(name)) return false;
	readObject(func);
	return true;
}

template<class FUNC> inline
void JsonReader::readObject(FUNC func) {
	beginObject();
	while (!endObject()) {
		auto pos = _source.pos();
		func();
		if (pos == _source.pos()) {
			skipUnhandledMember(true);
		}
	}
}

template<class FUNC> inline
bool JsonReader::readMemberArray(StrView name, FUNC func) {
	if (!isMember(name)) return false;
	readArray(func);
	return true;
}

template<class FUNC> inline
void JsonReader::readArray(FUNC func) {
	beginArray();
	while (!endArray()) {
		auto pos = _source.pos();

		func();

		if (pos == _source.pos()) {
			AX_ASSERT(false);
			skipValue();
		}
	}
}

template<class... ARGS> inline
void JsonReader::log(LogLevel lv, StrView fmtStr, ARGS&&... args) {
	TempString msg;
	FmtTo(msg, fmtStr, AX_FORWARD(args)...);
	FmtTo(msg, "\n  token=[{}]\n-----------\n", _token.str);

	_source.appendSourceLocation(msg, 8);
	Logger::s_get()->write(lv, msg);

	if (lv == LogLevel::Error) {
		throw Error_JsonReader(msg);
	}
}

template<class R> inline
void JsonReader::readValue(IString_<R>& outValue) {
	if (_valueType != ValueType::String) error("value is not String");
	outValue.setUtf(_token.str);
	next();
}

template<class R> inline
void JsonReader::readValue(MutSpan<R> outValue) {
	Int i = 0;
	readArray([&]() {
		if (i >= outValue.size()) {
			warning("too many array elements");
			return;
		}
		readValue(outValue[i]);
		i++;
	});

	if (i != outValue.size()) {
		warning("array size mismatch");
	}
}

template<class R> inline
void JsonReader::readValue(IArray<R>& outValue) {
	outValue.clear();
	readArray([&]() {
		readValue(outValue.emplaceBack());
	});
}

} // namespace
