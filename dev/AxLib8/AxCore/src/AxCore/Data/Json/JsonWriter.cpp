module;
module AxCore.JsonWriter;

namespace ax {

JsonWriter::JsonWriter(IString & outJson) {
	_json = &outJson;
}

JsonWriter::~JsonWriter() {
	if (_level.size() != 0) {
		assert(false); //"JsonWriter end within scope"
	}
}

void JsonWriter::beginObject(StrView name) {
	writeMemberName(name);
	beginObject();
}

void JsonWriter::beginObject() {
	preWriteValue();
	_json->append('{');
	_commaNeeded = false;
	_level.append(LevelType::Object);
}

void JsonWriter::endObject() {
	newline(-1);
	_json->append('}');
	_commaNeeded = true;

	if (_level.size() == 0) {
		throw Error_JsonWriter();
	}
	if (_level.back() != LevelType::Object) {
		throw Error_JsonWriter();
	}
	_level.decSize(1);
}

void JsonWriter::beginArray(StrView name) {
	writeMemberName(name);
	beginArray();
}

void JsonWriter::beginArray() {
	preWriteValue();
	_json->append('[');
	_commaNeeded = false;
	_level.append(LevelType::Array);
}

void JsonWriter::endArray() {
	_json->append(']');
	_commaNeeded = true;

	if (_level.size() == 0) {
		throw Error_JsonWriter();
	}
	if (_level.back() != LevelType::Array) {
		throw Error_JsonWriter();
	}
	_level.decSize(1);
}

void JsonWriter::writeMemberName(StrView name) {
	if (_level.size() == 0 || _level.back() != LevelType::Object) {
		throw Error_JsonWriter();
	}

	if (_commaNeeded)
		_json->append(", ");
	_commaNeeded = true;
//-------
	newline();
	_writeQuoteString(name);
	_json->append(": ");
	_commaNeeded = false;
	_level.append(LevelType::Member);
}

void JsonWriter::writeNull() {
	preWriteValue();
	_json->append("null");
}

void JsonWriter::writeValue(bool value) {
	preWriteValue();
	_json->append(ConstStrLit_bool(value));
}

void JsonWriter::writeValue(i64 value) {
	preWriteValue();
	_json->append(Fmt("{}", value));
}

void JsonWriter::writeValue(u64 value) {
	preWriteValue();
	_json->append(Fmt("{}", value));
}

void JsonWriter::writeValue(f64 value) {
	preWriteValue();
	if (Math::isNaN(value)) {
		_writeQuoteString("NaN");

	} else if (Math::isInfinity(value)) {
		_writeQuoteString("Infinity");

	} else if (Math::isNegInfinity(value)) {
		_writeQuoteString("-Infinity");

	} else {
		_json->append(Fmt("{}", value));
	}
}

void JsonWriter::writeValue(const JsonValue& value) {
	using Type = JsonValue::Type;

	switch (value.type()) {
		case Type::Undefined:	AX_ASSERT(false); writeValue(nullptr); break;
		case Type::Null:		writeValue(nullptr);			break;
		case Type::Bool:		writeValue(value.asBool());		break;
		case Type::Double:		writeValue(value.asDouble());	break;
		case Type::i64:			writeValue(value.asInt64());	break;
		case Type::u64:			writeValue(value.asUInt64());	break;
		case Type::String:		writeValue(value.asString());	break;
		case Type::Object:		writeValue(value.asObject());	break;
		case Type::Array:		writeValue(value.asArray());	break;
		default: AX_ASSERT(false);
	}
}

void JsonWriter::writeValue(const JsonObject& value) {
	auto scope = objectScope();
	for (auto& m : value.members) {
		writeMember(m.key(), m.value());
	}
}

void JsonWriter::writeValue(const JsonArray& value) {
	auto scope = arrayScope();
	for (auto& e : value) {
		writeValue(e);
	}
}

void JsonWriter::preWriteValue()
{
	if (_commaNeeded)
		_json->append(", ");
	_commaNeeded = true;

	if (_level.size() > 0) {
		auto lv = _level.back();
		switch (lv) {
		case LevelType::Member:
			_level.decSize(1);
			break;
		case LevelType::Array:
			break;
		case LevelType::Object:
			throw Error_JsonWriter();
			break;
		}
	}
}

template<class R> AX_INLINE
void JsonWriter::_writeQuoteUtfString(StrView_<R> v) {
	_json->append('\"');

	if constexpr(sizeof(R) == sizeof(Char)) {
		for (auto& ch : v) {
			_writeEncodeStringChar(ch);
		}
	} else {
		StringA tmp;
		for (auto& ch : v) {
			tmp.setUtf(ch);
			for (auto& t : tmp) {
				_writeEncodeStringChar(t);
			}
		}
	}
	_json->append('\"');
}

void JsonWriter::_writeQuoteString(StrViewA  v) { _writeQuoteUtfString(v); }
void JsonWriter::_writeQuoteString(StrViewW  v) { _writeQuoteUtfString(v); }
void JsonWriter::_writeQuoteString(StrView16 v) { _writeQuoteUtfString(v); }
void JsonWriter::_writeQuoteString(StrView32 v) { _writeQuoteUtfString(v); }

void JsonWriter::newline(Int offset) {
	if (_newlineNeeded) {
		_json->append('\n');
		Int n = _level.size() + offset;
		for(Int i = 0; i < n; i++) {
			_json->append("  ");
		}
	}
}

JsonWriter::ObjectScope::ObjectScope(JsonWriter* p)  {
	_p = p;
}

JsonWriter::ObjectScope::ObjectScope(ObjectScope && rhs) noexcept {
	_p = rhs._p;
	rhs._p = nullptr;
}

JsonWriter::ObjectScope::~ObjectScope() {
	if (_p) { _p->endObject(); _p = nullptr; }
}

JsonWriter::ArrayScope::ArrayScope(JsonWriter* p)  {
	_p = p;
}

JsonWriter::ArrayScope::ArrayScope(ArrayScope && rhs) noexcept {
	_p = rhs._p;
	rhs._p = nullptr;
}

JsonWriter::ArrayScope::~ArrayScope() {
	if (_p) { _p->endArray(); _p = nullptr; }
}

} // namespace