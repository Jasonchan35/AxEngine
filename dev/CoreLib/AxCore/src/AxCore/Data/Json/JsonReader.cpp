module;
#include "AxCore-pch.h"
module AxCore.JsonReader;

namespace ax {

JsonReader::JsonReader(StrView json, StrView filenameForErrorMessage) {
	readJson(json, filenameForErrorMessage);
}

void JsonReader::reset() {
	_source.reset();
	_valueType = ValueType::Invalid;
}

void JsonReader::readJson(StrView json, StrView filenameForErrorMessage) {
	reset();
	_source.init(json, filenameForErrorMessage);
	_nextChar();
	next();
}

void JsonReader::beginObject() {
	if (_valueType != ValueType::BeginObject) {
		error("ValueType is not BeginObject {{");
	}
	next();
}

bool JsonReader::beginObject(StrView name) {
	if (!isMember(name)) return false;
	beginObject();
	return true;
}

bool JsonReader::isEndObject() {
	return _valueType == ValueType::EndObject;
}

bool JsonReader::endObject() {
	if (_valueType != ValueType::EndObject) {
		if (onUnhandledMemeber()) return false;

		if (!_levels.size()) {
			error("invalid level when end object");
		}
		_levels.back().pos = _source.pos();
		if (_valueType != ValueType::EndObject) { // valueType may changed after skipValue
			return false;
		}
	}		
	next();
	return true;
}

bool JsonReader::unhandledMember() {
	if (_valueType == ValueType::EndObject) return false;
	if (_valueType != ValueType::Member) error("value is not member");

	if (!_levels.size()) {
		error("invalid level (object)");
	}
	
	auto& lv = _levels.back();
	if (lv.type != Level::Type::Object) {
		error("invalid level is not object");
	}
	
	if (lv.pos == _source.pos()) {
		return true;
	}
	return false;
}

bool JsonReader::skipUnhandledMember(bool showWarning) {
	if (!unhandledMember()) return false;
	if (showWarning) {
		warning("unhandled object member \"{}\"\n", _token.str);
	}
	skipMemberName();
	skipValue();
	return true;
}

bool JsonReader::onUnhandledMemeber() {
	if (!unhandledMember()) return false;
	warning("unhandled object member \"{}\"", _token.str);
	skipValue();
	return true;
}

void JsonReader::beginArray() {
	if (_valueType != ValueType::BeginArray) error("ValueType is not [");
	next();
}

bool JsonReader::beginArray(StrView name) {
	if (!isMember(name)) return false;
	beginArray();
	return true;
}

bool JsonReader::isEndArray() {
	return _valueType == ValueType::EndArray;
}

bool JsonReader::endArray() {
	if (_valueType != ValueType::EndArray) {
		if (!_levels.size()) {
			error("invalid level (array)");
		}
		return false;
	}
	next();
	return true;
}

bool JsonReader::expectEndArray() {
	if (_valueType == ValueType::EndArray)
		return true;

	warning("too many values in array");
	do {
		skipValue();
	} while (!endArray());

	return true;
}

bool JsonReader::isMember(StrView name) {
	if (_valueType == ValueType::EndObject) return false;
	if (_valueType != ValueType::Member) error("value is not member");
	if (_token.str != name) return false;
	next();
	return true;
}

StrView JsonReader::isMemberHasPrefix(StrView prefix) {
	if (_valueType != ValueType::Member) error("value is not member");
	auto name = StrView(_token.str);	
	auto suffix = name.extractFromPrefix(prefix);
	if (suffix) next();
	return suffix;
}

void JsonReader::readMemberName(IString& outValue) {
	if (_valueType != ValueType::Member) error("is not member name");
	peekMemberName(outValue);
	next();
}

bool JsonReader::peekMemberName(IString& outValue) {
	if (_valueType == ValueType::EndObject) return false;
	if (_valueType != ValueType::Member) error("is not member name");
	outValue.set(_token.str);
	return true;
}

void JsonReader::skipMemberName() {
	if (_valueType != ValueType::Member) error("is not member name");
	next();	
}

void JsonReader::readValue(bool& outValue) {
	if (_valueType != ValueType::Bool) error("value is not bool");
	outValue = _token.boolValue;
	next();
}

void JsonReader::readValue(JsonValue& outValue) {
	using InType = ValueType;
	auto type = valueType();
	switch (type) {
		case InType::f64:	{ f64 v = 0; readValue(v); outValue = v;			}break;
		case InType::u64:	{ u64 v = 0; readValue(v); outValue = v;			}break;
		case InType::i64:	{ i64 v = 0; readValue(v); outValue = v;			}break;
		case InType::Bool:	{ bool v = false; readValue(v); outValue = v;		}break;
		case InType::Null:	{ readNullValue(); outValue = nullptr;				}break;
		case InType::String:{ auto* s = outValue.setToString(); readValue(*s); }break;

		case InType::BeginObject: {
			auto* obj = outValue.setToObject();
			readValue(*obj);
		}break;
		case InType::BeginArray: {
			auto* arr = outValue.setToArray(0);
			readValue(*arr);
		}break;

		default: {
			error("unknown json value type");
		}break;
	}
}

void JsonReader::readValue(JsonObject&	outValue) {
	if (_valueType != ValueType::BeginObject) error("value is not object");
	
	TempString memberName;
	
	beginObject();
	while (!endObject()) {
		readMemberName(memberName);
		auto* m = outValue.getOrAddMember(memberName);
		readValue(*m);
	}
}

void JsonReader::readValue(JsonArray&	outValue) {
	if (_valueType != ValueType::BeginArray) error("value is not array");

	beginArray();
	while (!endArray()) {
		auto* e = outValue.emplaceBack();
		readValue(*e);
	}
}

void JsonReader::readNullValue() {
	if (_valueType != ValueType::Null) error("value is not null");
	next();
}

void JsonReader::skipValue() {
	switch (_valueType) {
		case ValueType::BeginArray: {
			beginArray();
			while (!endArray()) {
				skipValue();
			}
		}break;

		case ValueType::BeginObject: {
			beginObject();
			while (!endObject()) {
				skipValue();
			}
		}break;

		case ValueType::Member: {
			next();
			skipValue();
		}break;

		default: {
			next();
		}break;
	}
}

bool JsonReader::next() {
	auto lastValueType = _valueType;
	auto lastTokenType = _token.type;

	bool b = _nextToken();
	if (!b) return false;

	if (_currentLevelType() == Level::Type::Object) {
		if (lastValueType != ValueType::Member) {
			if (_token.type == TokenType::EndObject) {
				_levels.decSize(1);
				_valueType = ValueType::EndObject;
				return true;
			}

			if (lastTokenType != TokenType::BeginObject) {
				if (_token.type == TokenType::Comma) {
					_nextToken();
				} else {
					error("comma[,] expected between members");
				}
			}

			if (_token.type == TokenType::Identifier) {
				error("Quote [\"] is missing for member name [{}]", _token.str);

			} else if (_token.type != TokenType::String) {
				error("Member name must be string, token={}", _token.str);
			}

			_valueType = ValueType::Member;
			return true;
		}

		if (_token.type != TokenType::Colon) {
			error("colon[:] expected after member name");
		}
		_nextToken();

	} else if (_currentLevelType() == Level::Type::Array) {
		if (_token.type == TokenType::EndArray) {
			_levels.decSize(1);
			_valueType = ValueType::EndArray;
			return true;
		}

		if (lastTokenType != TokenType::BeginArray) {
			if (_token.type == TokenType::Comma) {
				_nextToken();
			} else {
				error("comma[,] expected between elements");
			}
		}
	}

	switch (_token.type) {
		case TokenType::BeginObject:
		{
			_levels.emplaceBack(Level::Type::Object, _source.pos());
			_valueType = ValueType::BeginObject;
			return true;
		}break;

		case TokenType::BeginArray:
		{
			_levels.emplaceBack(Level::Type::Array, _source.pos());
			_valueType = ValueType::BeginArray;
			return true;
		}break;

		case TokenType::Null:	 _valueType = ValueType::Null;		break;
		case TokenType::Bool:	 _valueType = ValueType::Bool;		break;
		case TokenType::String:	 _valueType = ValueType::String;	break;

		case TokenType::f64:	 _valueType = ValueType::f64;		break;
		case TokenType::u64:	 _valueType = ValueType::u64;		break;
		case TokenType::i64:	 _valueType = ValueType::i64;		break;

		default: error("Error invalid type in next()");
	}
	return true;
}

void JsonReader::dumpToken() {
	switch (_token.type) {
		case TokenType::Invalid:		AX_LOG("Token[Invalid]"); break;
		case TokenType::BeginObject:	AX_LOG("Token({)"); break;
		case TokenType::EndObject:		AX_LOG("Token(})"); break;
		case TokenType::BeginArray:		AX_LOG("Token([)"); break;
		case TokenType::EndArray:		AX_LOG("Token(])"); break;
		case TokenType::Null:			AX_LOG("Token(null)"); break;
		case TokenType::f64:			AX_LOG("Token(", _token.str, ")"); break;
		case TokenType::u64:			AX_LOG("Token(", _token.str, ")"); break;
		case TokenType::i64:			AX_LOG("Token(", _token.str, ")"); break;
		case TokenType::Bool:			AX_LOG("Token({}", _token.boolValue); break;
		case TokenType::String:			AX_LOG("Token(str=", _token.str, ")"); break;
		case TokenType::Comma:			AX_LOG("Token(,)"); break;
		case TokenType::Colon:			AX_LOG("Token(:)"); break;
		default:
			error("Unknown token type");
	}
}

void JsonReader::dumpValue() {
	switch (_valueType) {
		case ValueType::Invalid:		AX_LOG("JsValue[Invalid]"); break;
		case ValueType::BeginObject:	AX_LOG("JsValue({)"); break;
		case ValueType::EndObject:		AX_LOG("JsValue(})"); break;
		case ValueType::BeginArray:		AX_LOG("JsValue([)"); break;
		case ValueType::EndArray:		AX_LOG("JsValue(])"); break;
		case ValueType::Null:			AX_LOG("JsValue(null)"); break;
		case ValueType::f64:			AX_LOG("JsValue(", _token.str, ")"); break;
		case ValueType::u64:			AX_LOG("JsValue(", _token.str, ")"); break;
		case ValueType::i64:			AX_LOG("JsValue(", _token.str, ")"); break;
		case ValueType::Bool:			AX_LOG("JsValue({})", _token.boolValue); break;
		case ValueType::String:			AX_LOG("JsValue(str=", _token.str, ")"); break;
		case ValueType::Member:			AX_LOG("JsValue(member=", _token.str, ")"); break;
		default: error("Unknown value type"); break;
	}
}

bool JsonReader::_nextToken() {
	_valueType = ValueType::Invalid;
	_token.type = TokenType::Invalid;
	_token.str.clear();

	StrView spaces(" \t\n\r");

	for (;;) {
		while (spaces.findChar(_ch)) {
			_nextChar();
		}

		if (_ch == '/') {
			_nextChar();
			switch (_ch) {
				case '/': { 
					//trim inline comment
					_nextChar();
					while (_ch != '\n' && _ch != 0) {
						_nextChar();
					}
				}continue; //back to trim space again

				case '*': {
					//trim block comment
					_nextChar();
					for (;;) {
						if (_ch == 0) {
							error("Unexpected end of file in comment block");
						}

						if (_ch == '*') {
							_nextChar();
							if (_ch == '/') {
								_nextChar();
								break;
							}
						}else{
							_nextChar();
						}
					}
				}continue;

				default: error("Invalid token after '/'");
			}
		}

		break;
	}

	switch (_ch) {
		case 0: return false;
		case '{': _token.type = TokenType::BeginObject;	_nextChar(); return true;
		case '}': _token.type = TokenType::EndObject;	_nextChar(); return true;
		case '[': _token.type = TokenType::BeginArray;	_nextChar(); return true;
		case ']': _token.type = TokenType::EndArray;	_nextChar(); return true;
		case ',': _token.type = TokenType::Comma;		_nextChar(); return true;
		case ':': _token.type = TokenType::Colon;		_nextChar(); return true;
		case '"':
		{
			_token.type = TokenType::String;
			_parseStringToken();
			return true;
		};
	}

	for (;;) {
		if (isdigit(_ch) || isalpha(_ch) || _ch == '+' || _ch == '-' || _ch == '.') {
			_token.str << _ch;
			_nextChar();
		} else {
			break;
		}
	}

	if (_token.str == "null") {
		_token.type = TokenType::Null;
		return true;
	}

	if (_token.str == "true") {
		_token.type = TokenType::Bool;
		_token.boolValue = true;
		return true;
	}

	if (_token.str == "false") {
		_token.type = TokenType::Bool;
		_token.boolValue = false;
		return true;
	}

	if (_token.str.size() > 0) {
		auto c = _token.str[0];
		if (isdigit(c) || c == '+' || c == '-' || c == '.') {

			if (_token.str.findAnyChar(".")) {
				_token.type = TokenType::f64;
			} else if (c == '-') {
				_token.type = TokenType::u64;
			} else {
				_token.type = TokenType::i64;
			}
			return true;
		}

		if (isalpha(c) || c == '_') {
			_token.type = TokenType::Identifier;
			return true;
		}
	}

	error("error token=[{}]", _token.str);
	return false;
}

void JsonReader::_parseStringToken() {
	for (;;) {
		_nextChar();
		if (_ch == 0) {
			error("unexpected end of Json file");
		}

		switch (_ch) {
			case '"': _nextChar(); return;
				//escape char
			case '\\':
			{
				_nextChar();
				switch (_ch) {
					case '\\':
					case '/':
					case '"':
						_token.str << _ch;
						break;
					case 'b': _token.str << '\b'; break;
					case 'f': _token.str << '\f'; break;
					case 'n': _token.str << '\n'; break;
					case 'r': _token.str << '\r'; break;
					case 't': _token.str << '\t'; break;
					default:
						error("unexpected escape char '{}'", _ch);
						break;
				}
			}break;

			default: {
				_token.str << _ch;
			}break;
		}
	}
}

template<class V>
void JsonReader::_readNumericValue(V& outValue) {
	if constexpr (std::is_floating_point_v<V>) {
		if (valueIsString()) {
			if (_token.str == "NaN") {
				outValue = Math::NaN<V>;
				next();
				return;

			} else if (_token.str == "Infinity") {
				outValue = Math::infinity<V>;
				next();
				return;

			} else if (_token.str == "-Infinity") {
				outValue = Math::negInfinity<V>;
				next();
				return;
			}
		}
	}

	if (!valueIsNumber()) {
		error("value is not number");
	}

	if (!_token.str.tryParse(outValue)) {
		error("try parse number error");
	}
	next();
}

void JsonReader::readValue(i8&		outValue)	{ return _readNumericValue(outValue); }
void JsonReader::readValue(i16&		outValue)	{ return _readNumericValue(outValue); }
void JsonReader::readValue(i32&		outValue)	{ return _readNumericValue(outValue); }
void JsonReader::readValue(i64&		outValue)	{ return _readNumericValue(outValue); }

void JsonReader::readValue(u8&		outValue)	{ return _readNumericValue(outValue); }
void JsonReader::readValue(u16&		outValue)	{ return _readNumericValue(outValue); }
void JsonReader::readValue(u32&		outValue)	{ return _readNumericValue(outValue); }
void JsonReader::readValue(u64&		outValue)	{ return _readNumericValue(outValue); }

void JsonReader::readValue(f16&		outValue)	{ return _readNumericValue(outValue); }
void JsonReader::readValue(f32&		outValue)	{ return _readNumericValue(outValue); }
void JsonReader::readValue(f64&		outValue)	{ return _readNumericValue(outValue); }

} // namespace