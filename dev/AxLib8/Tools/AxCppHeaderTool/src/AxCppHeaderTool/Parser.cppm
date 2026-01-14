module;

export module AxCppHeaderTool:Parser;
export import :TypeInfo;

export namespace ax::AxCppHeaderTool {

using Source = LexerSource<Char>;

#define AX_HEADERTOOL_TokenType_ENUM_LIST(E) \
	E(None,) \
	E(Identifier,) \
	E(Number,) \
	E(String,) \
	E(Op,) \
	E(Newline,) \
//-----

AX_ENUM_CLASS_NO_STR(AX_HEADERTOOL_TokenType_ENUM_LIST, TokenType, u32)
} export namespace ax {
	AX_ENUM_STR_UTIL(AX_HEADERTOOL_TokenType_ENUM_LIST, ax::AxCppHeaderTool::TokenType);
} export namespace ax::AxCppHeaderTool {

class Parser : public NonCopyable {
public:
	void readFile(StrView filename, TypeDB& typeDB);

	bool matchIdentifier(StrView s);
	bool matchOp(StrView s);

	void parseExportModule();
	void parseNamespace();
	void parseUsing();
	void parseClass();
	void parseProp(TypeInfo& outType);
	void parseGeneratedBody(TypeInfo& outType);
	
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
		_source.appendSourceLocation(tmp, 5);
		AX_LOG("{}\n token=[{}]\n{}", tmp, _token.str, _source.location());
	}

	template<class... ARGS>
	void error(const FormatString<ARGS...>& fmt, const ARGS&... args) {
		log(fmt, args...);
		throw Error_Undefined();
	}
	
	struct Token {
		TokenType	type = TokenType::None;
		String		str;

		explicit operator bool() const		{ return type != TokenType::None; }
		bool isIdentifier() const			{ return type == TokenType::Identifier; }
		bool isIdentifier(StrView s) const	{ return type == TokenType::Identifier && str == s; }
		bool isOp() const					{ return type == TokenType::Op; }
		bool isOp(StrView s) const			{ return type == TokenType::Op && str == s; }
		bool isString() const				{ return type == TokenType::String; }
		bool isString(StrView s) const		{ return type == TokenType::String && str == s; }
		bool isNewline() const				{ return type == TokenType::Newline; }
		
		void setOp			(StrView s)		{ type = TokenType::Op; str = s; }
		void setString		(StrView s)		{ type = TokenType::String; str = s; }
		void setIdentifier	(StrView s)		{ type = TokenType::Identifier; str = s; }
		
		template<class CH>
		void onFormat(Format_<CH> & fmt) const {
			fmt << Fmt("{},[{}]", type, str);
		}
	};

	Token& token() { return _token; }
	
	const String& module() { return _module; }
	const String& modulePartition() { return _modulePartition; }

private:
	bool _nextToken();
	bool _parseIdentifier();
	bool _parseNumber();
	void appendQuotedString(IString& outStr, StrView inStr);

	void getNamespaceString(IString& outStr, IArray<String>& ns);

	void getOpenNamespaceScope (IString& outStr, IArray<String>& ns);
	void getCloseNamespaceScope(IString& outStr, IArray<String>& ns);

	Array<String> _namespaces;
	String	_module;
	String	_modulePartition;

	Token   _token;
	String  _data;
	Source  _source;
	Char&   _ch {_source.chRef()};
	TypeDB* _typeDB     = nullptr;
};



                                                                                                          } //namespace