module;

export module AxShaderTool:ShaderInfoParser;
export import AxRender;

export namespace ax /*::AxRender*/ {

class ShaderInfoParser : public NonCopyable {
public:
	using BlendFunc = RenderState::BlendFunc;

	void readFile(StrView outDir, StrView filename);

	bool nextToken();

	enum class TokenType {
		Unknown,
		Identifier,
		Number,
		String,
		Op,
		Newline,
	};

	struct Token {
		TokenType	type = TokenType::Unknown;
		String		str;

		explicit operator bool() const		{ return type != TokenType::Unknown; }
		bool isIdentifier() const			{ return type == TokenType::Identifier; }
		bool isIdentifier(StrView s) const	{ return type == TokenType::Identifier && str == s; }
		bool isOp() const					{ return type == TokenType::Op; }
		bool isOp(StrView s) const			{ return type == TokenType::Op && str == s; }
		bool isNumber() const				{ return type == TokenType::Number; }
		bool isString() const				{ return type == TokenType::String; }
		bool isString(StrView s) const		{ return type == TokenType::String && str == s; }
		bool isNewline() const				{ return type == TokenType::Newline; }
	};

	ShaderDeclareInfo	info;

	using Source = LexerSource<char>;
	const Source& source() { return _source; }

private:
	void _nextChar() { _ch = _source.nextChar(); }
	bool _nextToken();

	String _logString(StrView msg);

	template<class Format, class... Args>
	void log(FormatString<Args...> && fmt, const Args&... args) {
		_logString(Fmt(AX_FORWARD(fmt), args...));
	}

	template<class Format, class... Args>
	TempString logString(FormatString<Args...> && fmt, const Args&... args) {
		return _logString(Fmt(AX_FORWARD(fmt), args...));
	}

	template<class... Args>
	Error_Runtime _makeError(FormatString<Args...> && fmt, const Args&... args) {
		return Error_Runtime(_logString(Fmt(AX_FORWARD(fmt), args...)));
	}

	Error_Runtime _makeErrorUnexpectedChar();
	Error_Runtime _makeErrorUnexpectedToken();

	bool _parseIdentifier();
	bool _parseNumber();
	bool _parseString();
	void _parseCommentBlock();
	void _parseCommentInline();

	bool expectOp(StrView s);
	bool expectIdentifier(StrView s);

	void readShader();

	void readProperties();
	void readProperty();
	void readPropType(ShaderPropType& t);
	void readProperty_DefaultValue(ShaderPropInfo& prop);
	void readProperty_SamplerExtra(ShaderPropInfo& prop);

	void readPass();

	void readBlendFunc(BlendFunc& f);
	void readSamplerWrap(SamplerState& ss);

	template<class T> void readEnum(T& v);
	template<class V> void readNumber(V& v);

	template<class V> void readNumber(Num2_<V>& v);
	template<class V> void readNumber(Num3_<V>& v);
	template<class V> void readNumber(Num4_<V>& v);

	template<class V> void readColor(ColorRGB_<V>& v);
	template<class V> void readColor(ColorRGBA_<V>& v);

	void readIdentifier(IString& s);
	void readString(IString& s);
	void readBool(bool& v);

	bool matchIdentifier(StrView s)	{ if (!_token.isIdentifier(s))	return false; nextToken(); return true; }
	bool matchOp(StrView s)			{ if (!_token.isOp(s))			return false; nextToken(); return true; }
	bool matchNewline()				{ if (!_token.isNewline())		return false; nextToken(); return true; }

	FileMemMap	_fileMap;

	Source		_source;
	Char		_ch = 0;
	Token		_token;
};

template<class T> inline
void ShaderInfoParser::readEnum(T& v) {
	static_assert(std::is_enum_v<T>);

	if (!_token.isIdentifier()) {
		throw _makeErrorUnexpectedToken();
	}

	if (!EnumFn(v).tryParse(_token.str)) {
		throw _makeError("parse enum {}", _token.str);
	}
	nextToken();
}

template<class V> inline
void ShaderInfoParser::readNumber(V& v) {
	static_assert(std::is_integral_v<V> || std::is_floating_point_v<V> || std::is_same_v<V, f16>);

	if (!_token.isNumber()) throw _makeErrorUnexpectedToken();
	if (!_token.str.tryParse(v)) throw _makeErrorUnexpectedToken();

	nextToken();
}

template<class V> inline
void ShaderInfoParser::readNumber(Num2_<V>& v) {
	expectOp("[");	readNumber(v.x);
	expectOp(",");	readNumber(v.y);
	expectOp("]");
}

template<class V> inline
void ShaderInfoParser::readNumber(Num3_<V>& v) {
	expectOp("[");	readNumber(v.x);
	expectOp(",");	readNumber(v.y);
	expectOp(",");	readNumber(v.z);
	expectOp("]");
}

template<class V> inline
void ShaderInfoParser::readNumber(Num4_<V>& v) {
	expectOp("[");	readNumber(v.x);
	expectOp(",");	readNumber(v.y);
	expectOp(",");	readNumber(v.z);
	expectOp(",");	readNumber(v.w);
	expectOp("]");
}

template<class V> inline
void ShaderInfoParser::readColor(ColorRGB_<V>& v) {
	expectOp("[");	readNumber(v.r);
	expectOp(",");	readNumber(v.g);
	expectOp(",");	readNumber(v.b);
	expectOp("]");
}

template<class V> inline
void ShaderInfoParser::readColor(ColorRGBA_<V>& v) {
	expectOp("[");	readNumber(v.r);
	expectOp(",");	readNumber(v.g);
	expectOp(",");	readNumber(v.b);
	expectOp(",");	readNumber(v.a);
	expectOp("]");
}


} // namespace