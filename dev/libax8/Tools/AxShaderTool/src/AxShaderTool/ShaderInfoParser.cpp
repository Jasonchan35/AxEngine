module AxShaderTool;

import :ShaderInfoParser;

namespace ax::AxRender {

void ShaderInfoParser::readFile(StrView outdir, StrView filename) {
	_fileMap.openFile(filename);

	_source.init(_fileMap.strViewA(), filename);
	_nextChar();
	nextToken();

	if (!expectOp("#")) return;
	if (!expectIdentifier("if")) return;
	if (!expectIdentifier("ShaderInfo")) return;

	auto ext = FilePath::extension(filename);

	if (ext == "axShader") {
		readShader();
	}

	{
		String outFilename = outdir;
		outFilename.append("/info.json");

		JsonIO::writeFileIfChanged(outFilename, info, true);
	}
}

String ShaderInfoParser::_logString(StrView msg) {
	String tmp;

	tmp << msg;
	tmp << "\n------------\n";
	_source.appendSourceLocation(tmp, 8);
	Logger::s_get()->write(LogLevel::Error, msg);

	return tmp;
}



bool ShaderInfoParser::nextToken() {
	if (!_nextToken()) return false;
//	AX_DUMP_VAR(_token.str);
	return true;
}

bool ShaderInfoParser::_nextToken() {
	_token.type = TokenType::Unknown;
	_token.str.clear();

	for (;;) {
		while (_ch == ' '	|| _ch == '\t' || _ch == '\r') {
			_nextChar();
		}

		if (!_ch) return false;

		if (_ch == '_' || CharUtil::isAlpha(_ch)) {
			return _parseIdentifier();
		}

		if (CharUtil::isDigit(_ch)) return _parseNumber();
		if (_ch == '\"') return _parseString();

		if (_ch == '\n') {
			_token.type = TokenType::Newline;
			_token.str << "<newline>";
			_nextChar();
			return true;
		}

		if (_ch == '/') {
			_nextChar();
			if (_ch == '*') { 
				_parseCommentBlock();
				continue;
			}

			if (_ch == '/') { 
				_parseCommentInline();
				continue;
			}

			_token.type = TokenType::Op;
			_token.str << _ch;
			return true;
		}


		_token.type = TokenType::Op;
		_token.str << _ch;
		_nextChar();
		return true;
	}
}

Error_Runtime ShaderInfoParser::_makeErrorUnexpectedChar() {
	return _makeError("Unexpected character [{}]", _ch);
}

Error_Runtime ShaderInfoParser::_makeErrorUnexpectedToken() {
	return _makeError("Unexpected token [{}]", _token.str);
}

bool ShaderInfoParser::_parseIdentifier() {
	_token.type = TokenType::Identifier;

	_token.str += _ch;
	_nextChar();

	while (_ch) {
		if (_ch == '_' || CharUtil::isAlpha(_ch) || CharUtil::isDigit(_ch)) {
			_token.str += _ch;
			_nextChar();
		}else{
			break;
		}
	}
	return true;
}

bool ShaderInfoParser::_parseNumber() {
	_token.type = TokenType::Number;
	_token.str += _ch;
	_nextChar();

	bool hasDot = false;

	while(_ch) {
		if (_ch == '.') {
			if (hasDot) {
				throw _makeErrorUnexpectedChar();
			}
			hasDot = true;
			_token.str += _ch;
			_nextChar();

		}else if (CharUtil::isDigit(_ch)) {
			_token.str += _ch;
			_nextChar();

		}else if (_ch == 'f') {
			_token.str += _ch;
			_nextChar();
			break;

		} else {
			break;
		}
	}
	
	return true;
}

bool ShaderInfoParser::_parseString() {
	_token.type = TokenType::String;

	for (;;) {
		_nextChar();
		if (_ch == '\\') {
			_nextChar();
			switch (_ch) {
				case '\\':
				case '/':
				case '"':
					_token.str += _ch;
					break;
				case 'b': _token.str += '\b'; break;
				case 'f': _token.str += '\f'; break;
				case 'n': _token.str += '\n'; break;
				case 'r': _token.str += '\r'; break;
				case 't': _token.str += '\t'; break;
				default:
					throw Error_Undefined();
			}
		}else if (_ch == '\"') {
			_nextChar();
			break;
		}else{
			_token.str += _ch;
		}
	}
	return true;
}

void ShaderInfoParser::_parseCommentBlock() {
	_nextChar();			
	for(;;) {
		if (!_ch) return;
		if (_ch == '*') {
			_nextChar();
			if (_ch == '/') {
				_nextChar();
				return;
			}
		}else{
			_nextChar();
		}
	}
}

void ShaderInfoParser::_parseCommentInline() {
	_nextChar();			
	for(;;) {
		if (!_ch) return;
		if (_ch == '\n') {
			return;
		}else{
			_nextChar();
		}
	}
}

#if 0
#pragma mark "------------ read values ----------------"
#endif

bool ShaderInfoParser::expectOp(StrView s) {
	if (_token.isOp() && _token.str == s) {
		nextToken();
		return true;
	}
	throw _makeError("expected op token {}", s);
}

bool ShaderInfoParser::expectIdentifier(StrView s) {
	if (_token.isIdentifier() && _token.str == s) {
		nextToken();
		return true;
	}
	throw _makeError("expected identifier token {}", s);
}

void ShaderInfoParser::readIdentifier(IString& s) {
	if (!_token.isIdentifier()) throw _makeErrorUnexpectedToken();
	s = _token.str.view();
	nextToken();
}

void ShaderInfoParser::readString(IString& s) {
	if (!_token.isString()) throw _makeErrorUnexpectedToken();
	s = _token.str.view();
	nextToken();
}

void ShaderInfoParser::readBool(bool& v) {
	if (!_token.isIdentifier()) throw _makeErrorUnexpectedToken();
	if (_token.str == "true") {
		v = true;
	}else if(_token.str == "false") {
		v = false;
	}else{
		throw _makeErrorUnexpectedToken();
	}
	nextToken();
}

#if 0
#pragma mark "------------ read Shader ----------------"
#endif

void ShaderInfoParser::readShader() {
	for(;;) {
		if (matchOp("#")) {
			expectIdentifier("endif");
			return;
		}

		if (matchNewline()) { continue; }
		if (matchIdentifier("_GlobalCommonShader")) {
			info.isGlobalCommonShader = true;
			continue;
		}
		if (matchIdentifier("Properties")) { readProperties(); continue; }
		if (matchIdentifier("Pass")) { readPass(); continue; }

		throw _makeErrorUnexpectedToken();
	}
}

void ShaderInfoParser::readProperties() {
	expectOp("{");

	for(;;) {
		if (matchOp("}"))	break;
		if (matchNewline()) continue;
		if (_token.isIdentifier()) {
			readProperty();
			continue;
		}
		
		throw _makeErrorUnexpectedToken();
	}
}

void ShaderInfoParser::readPropType(ShaderPropType& t) {
// mapping HLSL type
	if (matchIdentifier("half"		))	{ t = ShaderPropType::f16;		return; }
	if (matchIdentifier("half2"		))	{ t = ShaderPropType::f16x2;	return; }
	if (matchIdentifier("half3"		))	{ t = ShaderPropType::f16x3;	return; }
	if (matchIdentifier("half4"		))	{ t = ShaderPropType::f16x4;	return; }

	if (matchIdentifier("float"		))	{ t = ShaderPropType::f32;		return; }
	if (matchIdentifier("float2"	))	{ t = ShaderPropType::f32x2;	return; }
	if (matchIdentifier("float3"	))	{ t = ShaderPropType::f32x3;	return; }
	if (matchIdentifier("float4"	))	{ t = ShaderPropType::f32x4;	return; }

	if (matchIdentifier("double"	))	{ t = ShaderPropType::f64;		return; }
	if (matchIdentifier("double2"	))	{ t = ShaderPropType::f64x2;	return; }
	if (matchIdentifier("double3"	))	{ t = ShaderPropType::f64x3;	return; }
	if (matchIdentifier("double4"	))	{ t = ShaderPropType::f64x4;	return; }

	readEnum(t);
}

void ShaderInfoParser::readProperty() {
	auto& prop = info.props.emplaceBack();

	readPropType(prop.propType);
	readIdentifier(prop.name);

	// optional display name
	if (_token.isString()) {
		readString(prop.displayName);
	}

	readProperty_DefaultValue(prop);
	readProperty_SamplerExtra(prop);

	if (!_token.isNewline()) {
		throw _makeErrorUnexpectedToken();
	}
	nextToken();
}

void ShaderInfoParser::readProperty_DefaultValue(ShaderPropInfo& prop) {
	// optional defaultValue
	if (matchOp("=")) {
		switch (prop.propType) {
		#define E(T,...) \
			case ShaderPropType::T:	readNumber(prop.defaultValue.v_##T); break; \
		//---
			AX_ShaderPropType_Numbers_EnumList(E)
		#undef E
			case ShaderPropType::Color3f:		readColor(prop.defaultValue.v_Color3f); break;
			case ShaderPropType::Color4f:		readColor(prop.defaultValue.v_Color4f); break;
			case ShaderPropType::Texture2D:		readEnum(prop.defaultValue.v_stockTextureId); break;
			case ShaderPropType::Texture3D:		readEnum(prop.defaultValue.v_stockTextureId); break;
			case ShaderPropType::TextureCube:	readEnum(prop.defaultValue.v_stockTextureId); break;

			default: throw _makeError("unhandled propType {}", prop.propType);
		}
	} else {
		switch (prop.propType) {
		#define E(T,...) \
			case ShaderPropType::T:  prop.defaultValue.v_##T = {}; break;
		//----
			AX_ShaderPropType_Numbers_EnumList(E)
		#undef E
			case ShaderPropType::Color3f:		prop.defaultValue.v_Color3f = Color3f::kWhite();	break;
			case ShaderPropType::Color4f:		prop.defaultValue.v_Color4f = Color4f::kWhite();	break;
			case ShaderPropType::Texture2D:		prop.defaultValue.v_stockTextureId = StockTextureId::White;	break;
			case ShaderPropType::Texture3D:		prop.defaultValue.v_stockTextureId = StockTextureId::White;	break;
			case ShaderPropType::TextureCube:	prop.defaultValue.v_stockTextureId = StockTextureId::White;	break;
			default: throw _makeError("unhandled propType {}", prop.propType);
		}
	}
}

void ShaderInfoParser::readProperty_SamplerExtra(ShaderPropInfo& prop) {
	if (!matchOp("{")) return;
	
	auto& ss = prop.samplerState;

	while (!matchOp("}")) {
		if (matchNewline()) continue;

		if (matchIdentifier("Asset")) {
			expectOp("=");
			readString(prop.asset);
			continue;
		}

		if (matchIdentifier("Filter")) {
			expectOp("=");
			readEnum(ss.filter);
			continue;
		}

		if (matchIdentifier("Wrap")) {
			expectOp("=");
			readSamplerWrap(ss);
			continue;
		}

		if (matchOp("}")) break;

		throw _makeErrorUnexpectedToken();
	}
}

void ShaderInfoParser::readBlendFunc(BlendFunc& f) {
	expectOp("[");	readEnum(f.op);
	expectOp(",");	readEnum(f.srcFactor);
	expectOp(",");	readEnum(f.dstFactor);
	expectOp("]");
}

void ShaderInfoParser::readPass() {
	auto& o = info.passes.emplaceBack();
	readString(o.name);

	if (o.name.size() < 1) {
		throw _makeError("pass name is missing");
	}

	for (auto& c : o.name) {
		if (isdigit(c) || isalpha(c) || c == '_') continue;
		throw _makeError("pass name '{}' contains invalid character {}", o.name, c);
	}

	expectOp("{");

	for(;;) {
		if (matchOp("}")) break;
		if (matchNewline()) continue;

		if (matchIdentifier("Cull")) {
			expectOp("=");
			readEnum(o.renderState.cull);
			continue;
		}
		if (matchIdentifier("AntialiasedLine")) {
			expectOp("=");
			readBool(o.renderState.antialiasedLine);
			continue;
		}
		if (matchIdentifier("Wireframe")) {
			expectOp("=");
			readBool(o.renderState.wireframe);
			continue;
		}
		if (matchIdentifier("DepthTest") ) {
			expectOp("=");

			DepthTestOp v;
			readEnum(v);
			o.renderState.depthTest.op = v;
			continue; 
		}
		if (matchIdentifier("DepthWrite")) {
			expectOp("=");
			bool b;
			readBool(b);
			o.renderState.depthTest.writeMask = b;
			continue;
		}
		if (matchIdentifier("BlendRGB")) {
			expectOp("=");
			readBlendFunc(o.renderState.blend.rgb);
			continue;
		}
		if (matchIdentifier("BlendAlpha")) {
			expectOp("=");
			readBlendFunc(o.renderState.blend.alpha);
			continue;
		}

		if (matchIdentifier("VsFunc")) { expectOp("="); readIdentifier(o.vsFunc); continue; }
		if (matchIdentifier("PsFunc")) { expectOp("="); readIdentifier(o.psFunc); continue; }
		if (matchIdentifier("GsFunc")) { expectOp("="); readIdentifier(o.gsFunc); continue; }

		throw _makeErrorUnexpectedToken();
	}
}

void ShaderInfoParser::readSamplerWrap(SamplerState& ss) {
	if (matchOp("[")) {
		readEnum(ss.wrapU);

		if (matchOp(",")) {
			readEnum(ss.wrapV);

			if (matchOp(",")) {
				readEnum(ss.wrapW);
			}
		}

		expectOp("]");
		return;
	}
	
	readEnum(ss.wrapU);
	ss.wrapV = ss.wrapU;
	ss.wrapW = ss.wrapU;
}

} // namespace
