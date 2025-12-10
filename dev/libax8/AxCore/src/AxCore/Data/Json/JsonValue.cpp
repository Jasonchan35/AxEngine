module;

module AxCore.JsonValue;

namespace ax {

void JsonValue::setNull() {
	_reset();
	_type = Type::Null;
}

void JsonValue::setUndefined() {
	_reset();
}

void JsonValue::setValue(bool b) {
	_reset();
	_type = Type::Bool;
	_value.v_bool = b;
}

void JsonValue::setValue(double v) {
	_reset();
	_type = Type::Double;
	_value.v_double = v;
}

void JsonValue::setValue(i64 v) {
	_reset();
	_type = Type::i64;
	_value.v_int64 = v;
}

void JsonValue::setValue(u64 v) {
	_reset();
	_type = Type::u64;
	_value.v_uint64 = v;
}

template<class CH> AX_INLINE
void JsonValue::_setValue(StrView_<CH> s) {
	setToString()->setUtf(s);
}

void JsonValue::setValue(StrViewA  s) { _setValue(s); }
void JsonValue::setValue(StrViewW  s) { _setValue(s); }
void JsonValue::setValue(StrView16 s) { _setValue(s); }
void JsonValue::setValue(StrView32 s) { _setValue(s); }

JsonObject* JsonValue::setToObject() {
	if (!isObject()) {
		_reset();
		_type = Type::Object;
		_value.v_object = new (AX_ALLOC_REQ) JsonObject;
	}
	return _value.v_object;
}

JsonArray* JsonValue::setToArray(Int size) {
	if (!isArray()) {
		_reset();
		_type = Type::Array;
		_value.v_array = new (AX_ALLOC_REQ) JsonArray;
	}
	auto& arr = *_value.v_array;
	arr.resize(size);
	return &arr;
}

IString* JsonValue::setToString() {
	if (!isString()) {
		_reset();
		_type = Type::String;
		_value.v_string = new (AX_ALLOC_REQ) String;
	}
	return _value.v_string;
}

void JsonValue::operator=(JsonValue && r) noexcept {
	_reset();
	_type  = r._type;
	_value = r._value;
	r._type  = Type::Undefined;
}

JsonValue* JsonValue::findMember(StrView name) {
	auto* obj = tryGetObject();
	if (!obj) return nullptr;
	return obj->findMember(name);
}

void JsonValue::_reset() {
	switch (_type) {
		case Type::String: ax_delete(_value.v_string); break;
		case Type::Object: ax_delete(_value.v_object); break;
		case Type::Array:  ax_delete(_value.v_array ); break;
	default: break;
	}
	_type = Type::Undefined;
}

bool JsonValue::tryGetValue(bool & outValue) const noexcept {
	if (_type != Type::Bool) return false;
	outValue = _value.v_bool;
	return true;
}

} // namespace