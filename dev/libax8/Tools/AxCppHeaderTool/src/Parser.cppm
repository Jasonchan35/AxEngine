module;

export module AxHeaderTool.Parser;
export import AxHeaderTool.TypeInfo;

export namespace ax::AxHeaderTool {

class Parser : public NonCopyable {
public:
	void readFile(StrView filename, TypeDB& typeDB);

	void parseNamespace();
	void parseClass();
	void parseProp(TypeInfo& outType);
	
	void skipBlock(IString* appendToStr = nullptr);

	void readExpectIdentifer(StrView s);
	void readExpectOp(StrView s);
	void readIdentifer(IString & s, StrView msg);
	void readValueString(IString& s, StrView msg);
	void readDefaultValue(IString & s, StrView msg);
	void readAppendTemplateParam(IString & outStr);

	bool nextToken();
	bool nextChar();

	void errorUnexpectedEndOfFile(StrView from);
	void errorUnexpectedChar();
	void errorUnexpectedToken();

	template<class... ARGS>
	void log(const FormatString<ARGS...>& fmt, const ARGS&... args) {
		TempString tmp;
		tmp.appendFormat(fmt, args...);
		AX_LOG("{}\n{}:{}\n_token=[{}]", tmp, _filename, _lineNumber, _token.str);
	}

	template<class... ARGS>
	void error(const FormatString<ARGS...>& fmt, const ARGS&... args) {
		log(fmt, args...);
		throw Error_Undefined();
	}

	enum class TokenType {
		Unknown,
		Identifier,
		Number,
		String,
		Op,
		Newline,
	};

	struct Token {
		TokenType	type;
		String		str;

		explicit operator bool() const			{ return type != TokenType::Unknown; }
		bool isIdentifier() const				{ return type == TokenType::Identifier; }
		bool isIdentifier(StrView s) const	{ return type == TokenType::Identifier && str == s; }
		bool isOp() const						{ return type == TokenType::Op; }
		bool isOp(StrView s) const			{ return type == TokenType::Op && str == s; }
		bool isString() const					{ return type == TokenType::String; }
		bool isString(StrView s) const		{ return type == TokenType::String && str == s; }
		bool isNewline() const					{ return type == TokenType::Newline; }
	};

	Token& token() { return _token; }

private:
	bool _nextToken();
	void _trimSpaces();
	bool _parseIdentifier();
	bool _parseNumber();
	bool _parseString();
	void _parseMultiLineComment();
	void _parseSingleLineComment();
	void appendQuotedString(IString& outStr, StrView inStr);

	void getNamespaceString(IString& outStr, IArray<String>& ns);

	void getOpenNamespaceScope (IString& outStr, IArray<String>& ns);
	void getCloseNamespaceScope(IString& outStr, IArray<String>& ns);

	Array<String> _namespaces;

	Token		_token;
	String		_source;
	String		_filename;
	Int			_lineNumber = 1;
	Char		_ch = 0;
	const Char*	_cur = nullptr;

	TypeDB*		_typeDB = nullptr;
};


} //namespace