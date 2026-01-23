module;


export module AxCore.JsonValue;
export import AxCore.Logger;
export import AxCore.Mat;
export import AxCore.Dict;

export namespace ax {


#define JsonValueType_ENUM_LIST(E) \
	E(Undefined,) \
	E(Null,) \
	E(Bool,) \
	E(Double,) \
	E(i64,) \
	E(u64,) \
	E(String,) \
	E(Object,) \
	E(Array,) \
//----

AX_ENUM_CLASS(JsonValueType_ENUM_LIST, JsonValueType, i8)

class JsonObject;
class JsonArray;

class JsonValue : public NonCopyable {
public:
	using Type = JsonValueType;

	JsonValue() = default;
	explicit JsonValue(JsonValue && r) noexcept { operator=(std::move(r)); }

	template<class V> explicit JsonValue(const V& v) { setValue(v); }

	~JsonValue() { _reset(); }

	Type	type() const { return _type; }

//---------
	bool isNull		() const { return _type == Type::Null;		}
	bool isUndefined() const { return _type == Type::Undefined; }
	bool isNullOrUndefined() const { return isNull() || isUndefined(); }

	bool isBool		() const { return _type == Type::Bool;		}
	bool isNumber	() const { return isDouble() || isInt64() || isUInt64(); }
	bool isDouble	() const { return _type == Type::Double;	}
	bool isInt64	() const { return _type == Type::i64;		}
	bool isUInt64	() const { return _type == Type::u64;	}

	bool isString	() const { return _type == Type::String;	}
	bool isObject	() const { return _type == Type::Object;	}
	bool isArray	() const { return _type == Type::Array;		}

	bool operator==(std::nullptr_t) const { return isNull(); }

	bool operator==(i8   v) const { return operator==(static_cast<i64>(v)); }
	bool operator==(i16  v) const { return operator==(static_cast<i64>(v)); }
	bool operator==(i32  v) const { return operator==(static_cast<i64>(v)); }
	bool operator==(i64  v) const { i64  tmp; if (!tryGetValue(tmp)) return false; return v == tmp; }

	bool operator==(u8  v) const { return operator==(static_cast<u64>(v)); }
	bool operator==(u16 v) const { return operator==(static_cast<u64>(v)); }
	bool operator==(u32 v) const { return operator==(static_cast<u64>(v)); }
	bool operator==(u64 v) const { u64 tmp; if (!tryGetValue(tmp)) return false; return v == tmp; }
	bool operator==(StrViewA v) const { if (!isString()) return false; return asString() == v; }
	
	bool operator==(const bool & v) const { if (!isBool()) return false; return asBool() == v; }

	template<class R, Int N>
	bool operator==(const R (&sz)[N]) const { return operator==(StrView(sz)); }

//---------
	void setUndefined();
	void setNull();

	void setValue(std::nullptr_t) { setNull(); }

	void setValue(bool      b);

	void setValue(f16  v) { setValue(v.to_f64()); }
	void setValue(f32  v) { setValue(static_cast<f64>(v)); }
	void setValue(f64  v);

	void setValue(i8   v) { setValue(static_cast<i64>(v)); }
	void setValue(i16  v) { setValue(static_cast<i64>(v)); }
	void setValue(i32  v) { setValue(static_cast<i64>(v)); }
	void setValue(i64  v);

	void setValue(u8   v) { setValue(static_cast<u64>(v)); }
	void setValue(u16  v) { setValue(static_cast<u64>(v)); }
	void setValue(u32  v) { setValue(static_cast<u64>(v)); }
	void setValue(u64  v);
	
	void setValue(StrViewA  s);
	void setValue(StrViewW  s);
	void setValue(StrView16 s);
	void setValue(StrView32 s);

	template<Int N> void setValue(const CharA  (&sz)[N]) { setValue(StrViewA (sz)); }
	template<Int N> void setValue(const CharW  (&sz)[N]) { setValue(StrViewW (sz)); }
	template<Int N> void setValue(const Char16 (&sz)[N]) { setValue(StrView16(sz)); }
	template<Int N> void setValue(const Char32 (&sz)[N]) { setValue(StrView32(sz)); }

	JsonObject*		setToObject();
	JsonArray*		setToArray(Int size);
	IString*		setToString();

	void operator=(JsonValue && r) noexcept;

	template<class R>
	void operator=(const R& r) { setValue(r); }

	JsonValue*	member(StrView name) {
		JsonValue* o = findMember(name);
		if (!o) throw Error_JsonReader(Fmt("no member {}", name));
		return o;
	}

	const JsonValue* member(StrView name) const { return ax_const_cast(this)->member(name); }
	
	      JsonObject& memberObject(StrView name)       { return member(name)->asObject(); }
	const JsonObject& memberObject(StrView name) const { return member(name)->asObject(); }
	      JsonArray&  memberArray (StrView name)       { return member(name)->asArray(); }
	const JsonArray&  memberArray (StrView name) const { return member(name)->asArray(); }
	      String&     memberString(StrView name)       { return member(name)->asString(); }
	const String&     memberString(StrView name) const { return member(name)->asString(); }
	
//---------
			JsonValue*	findMember(StrView name);
	const	JsonValue*	findMember(StrView name) const	{ return ax_const_cast(this)->findMember(name); }

	template<class VALUE>
	bool tryGetMemberValue(StrView memberName, VALUE& value) const;

			JsonObject*	tryGetMemberObject(StrView memberName);
	const	JsonObject*	tryGetMemberObject(StrView memberName) const	{ return ax_const_cast(this)->tryGetMemberObject(memberName); }

			JsonArray*	tryGetMemberArray (StrView memberName);
	const	JsonArray*	tryGetMemberArray (StrView memberName) const	{ return ax_const_cast(this)->tryGetMemberArray(memberName); }

			IString*	tryGetMemberStringPtr(StrView memberName);
	const	IString*	tryGetMemberStringPtr(StrView memberName) const	{ return ax_const_cast(this)->tryGetMemberStringPtr(memberName); }

	bool tryGetValue(IStringA  & outValue) const { return _tryGetStringValue(outValue); }
	bool tryGetValue(IStringW  & outValue) const { return _tryGetStringValue(outValue); }
	bool tryGetValue(IString16 & outValue) const { return _tryGetStringValue(outValue); }
	bool tryGetValue(IString32 & outValue) const { return _tryGetStringValue(outValue); }

	bool tryGetValue(bool	& outValue) const noexcept;

	bool tryGetValue(f32	& outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(f64	& outValue) const noexcept { return _tryGetNumericValue(outValue); }

	bool tryGetValue(i8		& outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(i16	& outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(i32	& outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(i64	& outValue) const noexcept { return _tryGetNumericValue(outValue); }

	bool tryGetValue(u8		& outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(u16	& outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(u32	& outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(u64	& outValue) const noexcept { return _tryGetNumericValue(outValue); }

	template<class R, VecSimd SIMD> bool tryGetValue(Vec1_< R, SIMD>& outValue) const noexcept;
	template<class R, VecSimd SIMD> bool tryGetValue(Vec2_< R, SIMD>& outValue) const noexcept;
	template<class R, VecSimd SIMD> bool tryGetValue(Vec3_< R, SIMD>& outValue) const noexcept;
	template<class R, VecSimd SIMD> bool tryGetValue(Vec4_< R, SIMD>& outValue) const noexcept;
	template<class R, VecSimd SIMD> bool tryGetValue(BBox2_<R, SIMD>& outValue) const noexcept;
	template<class R, VecSimd SIMD> bool tryGetValue(Quat4_<R, SIMD>& outValue) const noexcept;
	template<class R, VecSimd SIMD> bool tryGetValue(Mat4_< R, SIMD>& outValue) const noexcept;

			JsonObject*	tryGetObject()			{ return isObject() ? _value.v_object	: nullptr; }
	const	JsonObject*	tryGetObject() const	{ return isObject() ? _value.v_object	: nullptr; }

			JsonArray*	tryGetArray	()			{ return isArray()  ? _value.v_array	: nullptr; }
	const	JsonArray*	tryGetArray	() const	{ return isArray()  ? _value.v_array	: nullptr; }

			IString*	tryGetStringPtr()		{ return isString() ? _value.v_string	: nullptr; }
	const	IString*	tryGetStringPtr() const	{ return isString() ? _value.v_string	: nullptr; }

	template<class V>
	void getValue(V& outValue) {
		if (!tryGetValue(outValue)) { throw Error_Undefined(); }
	}

	template<class V>
	void getValue(V & outValue) const { ax_const_cast(this)->getValue(outValue); }

			bool&		asBool	()			{ _ensureType(Type::Bool   );	return _value.v_bool;    }
			double&		asDouble()			{ _ensureType(Type::Double );	return _value.v_double;  }
			i64&		asInt64	()			{ _ensureType(Type::i64 );		return _value.v_int64;   }
			u64&		asUInt64()			{ _ensureType(Type::u64 );		return _value.v_uint64;  }
			String&		asString()			{ _ensureType(Type::String );	return *_value.v_string; }
			JsonObject&	asObject()			{ _ensureType(Type::Object );	return *_value.v_object; }
			JsonArray&	asArray	()			{ _ensureType(Type::Array  );	return *_value.v_array;  }

	const	bool&		asBool	() const	{ _ensureType(Type::Bool   );	return _value.v_bool;    }
	const	double&		asDouble() const	{ _ensureType(Type::Double );	return _value.v_double;  }
	const	i64&		asInt64	() const	{ _ensureType(Type::i64 );		return _value.v_int64;   }
	const	u64&		asUInt64() const	{ _ensureType(Type::u64 );		return _value.v_uint64;  }
	const	String&		asString() const	{ _ensureType(Type::String );	return *_value.v_string; }
	const	JsonObject&	asObject() const	{ _ensureType(Type::Object );	return *_value.v_object; }
	const	JsonArray&	asArray	() const	{ _ensureType(Type::Array  );	return *_value.v_array;  }

//---------
	template<class CH>
	void onFormat(Format_<CH>& ctx) const {
		switch (_type) {
			case Type::Undefined:	AX_ASSERT(false);	break;
			case Type::Null:		ctx << nullptr;		break;
			case Type::Bool:		ctx << asBool();	break;
			case Type::Double:		ctx << asDouble();	break;
			case Type::i64:			ctx << asInt64();	break;
			case Type::u64:			ctx << asUInt64();	break;
			case Type::String:		ctx << asString();	break;
			case Type::Object:		ctx << asObject();	break;
			case Type::Array:		ctx << asArray();	break;
			default: AX_ASSERT(false);
		}
	}

private:
	AX_INLINE constexpr void _ensureType(Type t) const {
		if (t != _type) throw Error_JsonValue();
	}

	bool _tryGetValuePtr(JsonObject* & outValue) noexcept;
	bool _tryGetValuePtr(JsonArray*  & outValue) noexcept;
	bool _tryGetValuePtr(IStringA*   & outValue) noexcept;

	template<class CH> void _setValue(StrView_<CH> s);

	template<class VALUE>
	bool _tryGetNumericValue(VALUE & outValue) const noexcept {
		switch (_type) {
			case Type::Double:	outValue = static_cast<VALUE>(_value.v_double); return true;
			case Type::i64:		outValue = static_cast<VALUE>(_value.v_int64 ); return true;
			case Type::u64:		outValue = static_cast<VALUE>(_value.v_uint64); return true;
			default: return false;
		}
	}

	template<class STR>
	bool _tryGetStringValue(STR & str) const {
		str.clear();
		auto* p = tryGetStringPtr();
		if (!p) return false;
		str.setUtf(*p);
		return true;
	}
	
	void _reset();

	Type	_type = Type::Undefined;
	union Value {
		bool		v_bool;
		double		v_double;
		i64			v_int64;
		u64			v_uint64;
		String*		v_string;
		JsonObject*	v_object;
		JsonArray*	v_array;
	};
	Value _value {};
};

class JsonObject : public NonCopyable {
public:
	using Value  = JsonValue;

	JsonObject*	tryGetMemberObject(StrView memberName) {
		auto* m = findMember(memberName);
		if (!m) return nullptr;
		return m->tryGetObject();
	}
	const JsonObject* tryGetMemberObject(StrView memberName) const { return ax_const_cast(this)->tryGetMemberObject(memberName); }

	JsonArray*	tryGetMemberArray (StrView memberName) {
		auto* m = findMember(memberName);
		if (!m) return nullptr;
		return m->tryGetArray();
	}
	const JsonArray* tryGetMemberArray(StrView memberName) const { return ax_const_cast(this)->tryGetMemberArray(memberName); }

	IString*	tryGetMemberStringPtr(StrView memberName) {
		auto* m = findMember(memberName);
		if (!m) return nullptr;
		return m->tryGetStringPtr();
	}
	const IString* tryGetMemberStringPtr(StrView memberName) const { return ax_const_cast(this)->tryGetMemberStringPtr(memberName); }

	Value* findMember(StrView name) {
		auto* m = members.find(name);
		if (!m) return nullptr;
		return m;
	}
	const Value* findMember(StrView name) const { return ax_const_cast(this)->findMember(name); }
	
	JsonValue*	member(StrView name) {
		JsonValue* o = findMember(name);
		if (!o) throw Error_JsonReader(Fmt("no member {}", name));
		return o;
	}
	const JsonValue* member(StrView name) const { return ax_const_cast(this)->member(name); }
	
	      JsonObject& memberObject(StrView name)       { return member(name)->asObject(); }
	const JsonObject& memberObject(StrView name) const { return member(name)->asObject(); }
	      JsonArray&  memberArray (StrView name)       { return member(name)->asArray(); }
	const JsonArray&  memberArray (StrView name) const { return member(name)->asArray(); }
	      String&     memberString(StrView name)       { return member(name)->asString(); }
	const String&     memberString(StrView name) const { return member(name)->asString(); }
	
	Value* getOrAddMember(StrView name) {
		if (auto* m = members.find(name)) return m;
		return &members.add(name);
	}

	Value& operator[](StrView name) { return *getOrAddMember(name); }

	template<class CH>
	void onFormat(Format_<CH> & ctx) const {
		ctx << "{";
		{
			auto indent = ctx.indentScope();
			for (auto& e : members) {
				ctx.newline();
				ctx << e.key() << " : " << e.value();
			}
		}
		ctx.newline();
		ctx << "}";
	}

	Dict<String, Value> members;

friend class JsonValue;
protected:
	JsonObject() = default;
};

class JsonArray : public Array<JsonValue, 4> {
	using Base = Array<JsonValue, 4>;
public:
	template<class CH>
	void onFmt(Format_<CH>& ctx) const {
		ctx << *static_cast<const Base*>(this);
	}

friend class JsonValue;
protected:
	JsonArray() = default;
};

template <class FMT_CH> 
class FormatHandler<JsonArray, FMT_CH> : public FormatHandler<Span<JsonValue>, FMT_CH> {}; 

//--------- inline -----------------------

template<class VALUE> inline
bool JsonValue::tryGetMemberValue(StrView memberName, VALUE& value) const {
	auto* m = findMember(memberName);
	if (!m) return false;
	return m->tryGetValue(value);
}

inline
JsonObject*	JsonValue::tryGetMemberObject(StrView memberName) {
	auto* m = findMember(memberName);
	if (!m) return nullptr;
	return m->tryGetObject();
}

inline
JsonArray*	JsonValue::tryGetMemberArray (StrView memberName) {
	auto* m = findMember(memberName);
	if (!m) return nullptr;
	return m->tryGetArray();
}

inline
IString* JsonValue::tryGetMemberStringPtr(StrView memberName) {
	auto* m = findMember(memberName);
	if (!m) return nullptr;
	return m->tryGetStringPtr();
}

template<class R, VecSimd R_SIMD> inline
bool JsonValue::tryGetValue(Vec1_<R, R_SIMD>& outValue) const noexcept {
	if (!isArray()) return false;
	auto& arr = asArray();
	if (arr.size() != 1) return false;

	if (!arr[0].tryGetValue(outValue.cx)) return false;
	return true;
}

template<class R, VecSimd R_SIMD> inline
bool JsonValue::tryGetValue(Vec2_<R, R_SIMD>& outValue) const noexcept {
	if (!isArray()) return false;
	auto& arr = asArray();
	if (arr.size() != 2) return false;

	if (!arr[0].tryGetValue(outValue.x)) return false;
	if (!arr[1].tryGetValue(outValue.y)) return false;
	return true;
}

template<class R, VecSimd R_SIMD> inline
bool JsonValue::tryGetValue(Vec3_<R, R_SIMD>& outValue) const noexcept {
	if (!isArray()) return false;
	auto& arr = asArray();
	if (arr.size() != 3) return false;

	if (!arr[0].tryGetValue(outValue.x)) return false;
	if (!arr[1].tryGetValue(outValue.y)) return false;
	if (!arr[2].tryGetValue(outValue.z)) return false;
	return true;
}

template<class R, VecSimd R_SIMD> inline
bool JsonValue::tryGetValue(Vec4_<R, R_SIMD>& outValue) const noexcept {
	if (!isArray()) return false;
	auto& arr = asArray();
	if (arr.size() != 4) return false;

	if (!arr[0].tryGetValue(outValue.x)) return false;
	if (!arr[1].tryGetValue(outValue.y)) return false;
	if (!arr[2].tryGetValue(outValue.z)) return false;
	if (!arr[3].tryGetValue(outValue.w)) return false;
	return true;
}

template<class R, VecSimd R_SIMD> inline
bool JsonValue::tryGetValue(BBox2_<R, R_SIMD>& outValue) const noexcept {
	if (!isArray()) return false;
	auto& arr = asArray();
	if (arr.size() != 4) return false;

	if (!arr[0].tryGetValue(outValue.min)) return false;
	if (!arr[1].tryGetValue(outValue.max)) return false;
	return true;
}

template<class R, VecSimd R_SIMD> inline
bool JsonValue::tryGetValue(Quat4_<R, R_SIMD>& outValue) const noexcept {
	if (!isArray()) return false;
	auto& arr = asArray();
	if (arr.size() != 4) return false;

	if (!arr[0].tryGetValue(outValue.x)) return false;
	if (!arr[1].tryGetValue(outValue.y)) return false;
	if (!arr[2].tryGetValue(outValue.z)) return false;
	if (!arr[3].tryGetValue(outValue.w)) return false;
	return true;
}

template<class R, VecSimd R_SIMD> inline
bool JsonValue::tryGetValue(Mat4_<R, R_SIMD>& outValue) const noexcept {
	if (!isArray()) return false;
	auto& arr = asArray();
	if (arr.size() != 4) return false;

	if (!arr[0].tryGetValue(outValue.cx)) return false;
	if (!arr[1].tryGetValue(outValue.cy)) return false;
	if (!arr[2].tryGetValue(outValue.cz)) return false;
	if (!arr[3].tryGetValue(outValue.cw)) return false;
	return true;
}

} // namespace