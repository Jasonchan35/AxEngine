module;

module AxHeaderTool.Parser;

namespace ax::AxHeaderTool {

void Parser::readFile(StrView filename, TypeDB& typeDB) {
	AX_LOG("Parser::readFile {}", filename);
	
	_typeDB = &typeDB;

	File::readUtf8(filename, _data);
	_source.init(_data, filename);
	_source.trim(UtfUtil::kBOM);
	
	nextChar();
	nextToken();
	
	parseNamespace();
}

void Parser::parseNamespace() {
	do {
		if (_token.isOp("}")) break;

		if (_token.isNewline()) return;

		if (_token.isIdentifier("AX_CLASS")) {
			parseClass();
			continue;
		}
		if (_token.isIdentifier("namespace")) {
			nextToken();

			for (;;) {
				
				
				TempString tmp;
				readIdentifer(tmp, "namespace name");
				_namespaces.emplaceBack(tmp);
			
				if (_token.isOp("::")) {
					if (nextToken())
						errorUnexpectedEndOfFile("namespace");
					continue;
				}
				
				break;
			}
			
			readExpectOp("{");
			parseNamespace();
			continue;
		}
	} while (nextToken());

	if (_namespaces.size()) {
		_namespaces.decSize(1);
	}
}

void Parser::parseClass() {
	nextToken();
	readExpectOp("(");
	while (!_token.isOp(")")) {
		if (!nextToken()) {
			errorUnexpectedEndOfFile("CLASS attributes");
		}
	}
	nextToken();
	readExpectIdentifer("class");

	TempString tmp;
	readIdentifer(tmp, "class name");

	if (_typeDB->types.find(tmp)) {
		error("class {} already exists", tmp);
	}
	auto& outType = _typeDB->types.add(tmp);
	outType.name = tmp;

	//--- namespace scope ---
	getOpenNamespaceScope (outType.openNamespaceScope,  _namespaces);
	getCloseNamespaceScope(outType.closeNamespaceScope, _namespaces);

	//--- fullname ---
	getNamespaceString(outType.fullname, _namespaces);
	if (outType.fullname) {
		outType.fullname.append("::");
	}
	outType.fullname.append(tmp);
	//-------

	if (_token.isOp(":")) {
		nextToken();
		readExpectIdentifer("public");	
		readIdentifer(tmp, "base class");
		outType.baseName = tmp;		
	}

	if (_token.isOp(",")) {
		nextToken();

		if (_token.isIdentifier("public") || _token.isIdentifier("protected") || _token.isIdentifier("private")) {
			nextToken();
		}
		readIdentifer(tmp, "other base class");

		if (_token.isOp("<")) {
			skipBlock();
		}
	}

// class Body
	readExpectOp("{");
	while (!_token.isOp("}")) {
		if (_token.isIdentifier("AX_PROP")) {
			parseProp(outType);
			continue;
		}

		if (_token.isOp("{")) {
			skipBlock();
			continue;
		}

		if (!nextToken()) {
			errorUnexpectedEndOfFile("CLASS body");
		}
	}
}

void Parser::parseProp(TypeInfo& outType) {
	PropInfo::HasAttr	hasAttr;
	Array<Attribute>	attributes;

	nextToken();
	readExpectOp("(");
	for (;;) {
		if (_token.isOp(")")) break;

		auto& attr = attributes.emplaceBack();
		readIdentifer(attr.name, "read prop attribute");

		if (_token.isOp("(") || _token.isOp("{")) {
			readValueString(attr.defaultValue, "read prop attribute default value");
		}

		if (attr.name == "DontSave") {
			hasAttr.dontSave = true;
		}

		if (_token.isOp(",")) {
			nextToken();
		}
	}
	nextToken();

	for(;;) {
		if (_token.isIdentifier("static")) {
			error("doesn't support static property");
		}
		if (_token.isIdentifier("const")) {
			error("doesn't support const property");
		}
		break;
	}

	TempString typeName;
	readIdentifer(typeName, "property typename");

	if (_token.isOp("<")) {
		readAppendTemplateParam(typeName);
	}

	if (_token.isOp("*")) {
		error("doesn't support pointer property");
	}

	TempString varName;
	readIdentifer(varName, "property name");

	TempString defaultValue;
	readDefaultValue(defaultValue, "property defaultValue");

	TempString propName;
	TempString displayName;
	{
		auto tmp = varName.view();
		if (tmp.startsWith("_")) {
			tmp = tmp.sliceFrom(1);
		}
		propName = tmp;
		convertToDisplayName(displayName, tmp);
	}

	if (outType.props.find(propName)) {
		error("property [{}.{}] already exists", outType.name, displayName);
	}

	auto& prop = outType.props.add(propName);
	prop.name			= propName;
	prop.varName		= varName;
	prop.displayName	= displayName;
	prop.typeName		= typeName;
	prop.propType		= PropType::Normal;
	prop.defaultValue	= defaultValue;
	prop.hasAttr		= hasAttr;
	prop.attributes		= attributes;

	while (!_token.isOp(";")) {
		if (!nextToken()) {
			errorUnexpectedEndOfFile("Property ';'");
		}		
	}
}

void Parser::readAppendTemplateParam(IString & outStr) {
	outStr.append("<");
	readExpectOp("<");
	while (!_token.isOp(">")) {
		if (_token.isOp("<")) {
			readAppendTemplateParam(outStr);
			continue;
		}
		outStr.append(_token.str);

		if (!nextToken()) {
			errorUnexpectedEndOfFile("AX_PROP typename");
		}
	}
	outStr.append(">");
	nextToken();
}

void Parser::skipBlock(IString* appendToStr) {
	StrLit start;
	StrLit end;
	if (_token.str == "{") {
		start = "{";
		end = "}";
	}else if (_token.str == "(") {
		start = "(";
		end = ")";
	}else if (_token.str == "[") {
		start = "[";
		end = "]";
	}else if (_token.str == "<") {
		start = "<";
		end = ">";
	}else{
		error("unknown block begin {}", _token.str);
	}

	if (appendToStr) {
		appendToStr->append(_token.str);
	}

	nextToken();
	for(;;) {
		if (appendToStr) {
			appendToStr->append(_token.str);
		}

		if (_token.isOp(start)) {
			skipBlock(appendToStr);
			continue;
		}

		if (_token.isOp(end)) {
			nextToken();
			return;
		}

		if (!nextToken()) {
			TempString msg("skipBlock ", start);
			errorUnexpectedEndOfFile(msg);
		}
	}	
}

void Parser::readExpectIdentifer(StrView s) {
	if (!_token.isIdentifier(s)) {
		error("token [{}] expected", s);
	}
	nextToken();
}

void Parser::readExpectOp(StrView s) {
	if (!_token.isOp(s)) {
		error("token [{}] expected, current token=[{}]", s, _token);
	}
	nextToken();
}

void Parser::readIdentifer(IString & s, StrView msg) {
	if (!_token.isIdentifier()) {
		error("error read {}", msg);
	}
	s = _token.str;
	nextToken();
}

void Parser::readValueString(IString & s, StrView msg) {
	s.clear();

	StrLit	beginOp = "";
	StrLit	endOp   = "";

	if (_token.isOp("{")) {
		beginOp = "{";
		endOp   = "}";
	} else if (_token.isOp("(")) {
		beginOp = "(";
		endOp   = ")";
	}

	if (beginOp) {
		s.append(_token.str);
		nextToken();

		Int lv = 1;
		for (;;) {
			if (_token.isOp(beginOp)) {
				s.append(_token.str);
				nextToken();
				lv++;
				continue;
			}

			if (_token.isOp(endOp)) {
				s.append(_token.str);
				nextToken();
				lv--;
				if (lv <= 0)
					break;
				continue;
			}

			s.append(_token.str);
			nextToken();
		}		
	} else {
		s.append(_token.str);
		nextToken();
	}
}

void Parser::readDefaultValue(IString & s, StrView msg) {
	s.clear();
	if (_token.isOp(";")) {
		return;
	}

	if (_token.isOp("=")) {
		nextToken();
		for (;;) {
			if (_token.isOp(";")) break;
			s.append(_token.str);
			nextToken();
		}

	} else if (_token.isOp("{")) {
		readValueString(s, msg);
	}
}

bool Parser::nextToken() {
	if (!_nextToken()) return false;
//	AX_DUMP(_token.str);
	return true;
}

bool Parser::_nextToken() {
	_token.type = TokenType::Unknown;
	_token.str.clear();

	for (;;) {
		_trimSpaces();
		if (!_ch) return false;

		if (_ch == '_' || CharUtil::isAlpha(_ch)) {
			return _parseIdentifier();
		}

		if (_ch == '+' || _ch == '-' || CharUtil::isDigit(_ch)) {
			return _parseNumber();
		}

		if (_ch == '\"') return _parseString();
		if (_ch == '/') {
			nextChar();			
			if (_ch == '*') { 
				_parseMultiLineComment();
				continue;
			}

			if (_ch == '/') { 
				_parseSingleLineComment();
				continue;
			}
			
			_token.setOp(_ch);
			return true;
		}
		
		if (_ch == ':') {
			nextChar();
			if (_ch == ':') {
				_token.setOp("::");
				return true;
			}
			
			_token.setOp(_ch);
			return true;
		}

		_token.type = TokenType::Op;
		_token.str << _ch;
		nextChar();
		return true;
	}
}

void Parser::errorUnexpectedChar() {
	error("Unexpected character [{}]", _ch);
}

void Parser::errorUnexpectedToken() {
	error("Unexpected token [{}]", _token.str);
}

bool Parser::_parseIdentifier() {
	_token.type = TokenType::Identifier;

	_token.str << _ch;
	nextChar();

	while (_ch) {
		if (_ch == '_' || CharUtil::isAlpha(_ch) || CharUtil::isDigit(_ch)) {
			_token.str << _ch;
			nextChar();
		}else{
			break;
		}
	}
	return true;
}

bool Parser::_parseNumber() {
	_token.type = TokenType::Number;
	_token.str << _ch;
	nextChar();

	bool hasDot = false;

	while(_ch) {
		if (_ch == '.') {
			if (hasDot) {
				errorUnexpectedChar();
			}
			hasDot = true;
			_token.str << _ch;
			nextChar();
		}else if (CharUtil::isDigit(_ch)) {
			_token.str << _ch;
			nextChar();
		}else{
			break;
		}
	}
	
	return true;
}

bool Parser::_parseString() {
	_token.type = TokenType::String;

	for (;;) {
		nextChar();
		if (_ch == '\\') {
			nextChar();
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
					throw Error_Undefined();
			}
		}else if (_ch == '\"') {
			nextChar();
			break;
		}else{
			_token.str << _ch;
		}
	}
	return true;
}

void Parser::appendQuotedString(IString& outStr, StrView inStr) {
	outStr.append('"');
	for (auto& ch : inStr) {
		switch (ch) {
			case '\\':	outStr.append("\\\\"); break;
			case '\"':	outStr.append("\\\""); break;
			case '\b':	outStr.append("\\b"); break;
			case '\f':	outStr.append("\\f"); break;
			case '\n':	outStr.append("\\n"); break;
			case '\r':	outStr.append("\\r"); break;
			case '\t':	outStr.append("\\t"); break;
			default:	outStr.append(ch); break;
		}		
	}
	outStr.append('"');
}

void Parser::getOpenNamespaceScope(IString& outStr, IArray<String>& ns) {
	outStr.clear();
	if (ns.size()) {
		for (auto& e : ns) {
			outStr.append("namespace ", e, "{ ");
		}
		outStr.append("\n\n");
	}
}

void Parser::getCloseNamespaceScope(IString& outStr, IArray<String>& ns) {
	outStr.clear();
	if (ns.size()) {
		for (auto& e : ns) {
			AX_UNUSED(e);
			outStr.append("}");
		}
		outStr.append(" // namespace \n");
	}
}

void Parser::getNamespaceString(IString& outStr, IArray<String>& ns) {
	outStr.clear();
	for (auto& e : ns) {
		if (outStr.size()) {
			outStr.append("::");
		}
		outStr.append(e);
	}
}

void Parser::_parseMultiLineComment() {
	nextChar();			
	for(;;) {
		if (!_ch) return;
		if (_ch == '*') {
			nextChar();
			if (_ch == '/') {
				nextChar();
				return;
			}
		}else{
			nextChar();
		}
	}
}

void Parser::_parseSingleLineComment() {
	nextChar();			
	for(;;) {
		if (!_ch) return;
		if (_ch == '\n') {
			nextChar();
			return;
		}else{
			nextChar();
		}
	}
}

bool Parser::nextChar() {
	_ch = _source.nextChar();
	return _ch != 0;
}

void Parser::errorUnexpectedEndOfFile(StrView from) {
	error("unexpected end of file for {}", from);
}

void Parser::_trimSpaces() {
	for (;;) {
		if (_ch == ' '	|| _ch == '\t' || _ch == '\r' || _ch == '\n') {
			nextChar();
		}else{
			break;
		}
	}
}

} //namespace
