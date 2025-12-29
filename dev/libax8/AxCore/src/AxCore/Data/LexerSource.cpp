module;

module AxCore.LexerSource;

namespace ax {

template<class CH>
void LexerSource<CH>::reset() {
	_source		= {};
	_filename	= {};
	_remain     = {};
	_lineNumber = 1;
	_column		= 1;
	_ch         = 0;
}

template<class CH>
void LexerSource<CH>::init(StrView_<CH> source, StrView filename) {
	reset();
	_filename = filename;
	_source	  = source;
	_remain   = source;
	_ch       = _remain.size() > 0 ? _remain[0] : 0;
}

template<class CH>
bool LexerSource<CH>::trim(StrView_<CH> s) {
	if (startsWith(s)) {
		advancePos(s.size());
		return true;
	}
	return false;
}

template<class CH>
void LexerSource<CH>::skipUntil(StrView_<CH> delimiter, bool keepDelimiter) {
	while (_ch) {
		if (startsWith(delimiter)) {
			if (!keepDelimiter) {
				advancePos(delimiter.size());
			}
			return;
		}
		nextChar();
	}
}

template<class CH>
void LexerSource<CH>::read_c_string_literal(IString_<CH>& outStr) {
	if (_ch != '\"') throw Error_Undefined();
	nextChar();

	outStr.clear();
	for (;;) {
		if (_ch == 0) throw Error_Undefined("unexpected end of file");
		
		if (_ch == '\\') {
			nextChar();
			
			// \nnn octal
			if (CharUtil::isDigit(_ch)) { outStr += readOctal(3); continue; }
			if (_ch == 'u') { outStr += readHex(4); continue; } // \uHHHH
			if (_ch == 'U') { outStr += readHex(8); continue; } // \UHHHHHHHH
			
			// \xHH...
			if (_ch == 'x') {
				nextChar();
				CH v = 0;
				while (auto h = CharUtil::hexToByte(_ch)) {
					v <<= 4;
					v |= h.value();
				}
				outStr.append(v);
				continue;
			}
			
			switch (_ch) {
				case '\\': outStr += _ch;  nextChar(); continue;
				case  '/': outStr += _ch;  nextChar(); continue;
				case  '"': outStr += _ch;  nextChar(); continue;
				case  '?': outStr += _ch;  nextChar(); continue;
				case  'a': outStr += '\a'; nextChar(); continue;
				case  'b': outStr += '\b'; nextChar(); continue;
//				case  'e': outStr += '\e'; nextChar(); continue;
				case  'f': outStr += '\f'; nextChar(); continue;
				case  'n': outStr += '\n'; nextChar(); continue;
				case  'r': outStr += '\r'; nextChar(); continue;
				case  't': outStr += '\t'; nextChar(); continue;
				case  'v': outStr += '\v'; nextChar(); continue;
				default: throw Error_Undefined();
			}
		}else if (_ch == '\"') {
			nextChar();
			break;
			
		}else{
			outStr += _ch;
			nextChar();
			continue;
		}
	}
}

template<class CH>
CH LexerSource<CH>::readHex(Int n) {
	auto tmp = _remain.slice(0, n);
	CH   v   = 0;
	for (auto h : tmp) {
		auto o = CharUtil::hexToByte(h);
		if (!o) throw Error_Undefined();
		v <<= 4;
		v |= o.value();
	}
	advancePos(n);
	return v;
}

template<class CH>
CH LexerSource<CH>::readOctal(Int n) {
	auto tmp = _remain.slice(0, n);
	CH   v   = 0;
	for (auto h : tmp) {
		auto o = CharUtil::octalToByte(h);
		if (!o) throw Error_Undefined();
		v <<= 3;
		v |= o.value();
	}
	advancePos(n);
	return v;
}

template<class CH>
StrView_<CH> LexerSource<CH>::getLastFewLines(Int n, Int* outCount) {
	if (_remain.size() <= 0) return StrView_<CH>();

	Int p = pos();

	Int found = 0;
	while (_source.inBound(p) && found < n) {
		if (_source[p] == '\n')
			found++;
		if (p <= 0) break;
		--p;
	}

	if (_source.inBound(p) && _source[p] == '\n')
		++p;

	if (outCount) *outCount = found;

// find end of this line
	Int e = pos();
	while (_source.inBound(e) && _source[e] != '\n')
		++e;
	if (!_source.inBound(e)) --e;

	return StrView_<CH>(_source.slice(p, e - p));
}

template<class CH>
void LexerSource<CH>::appendSourceLocation(IString_<CH>& outStr, Int dumpLineCount) {
	Int lineCount = 0;
	auto lastFewLines = getLastFewLines(dumpLineCount, &lineCount);

	auto writeLineNum = _lineNumber - lineCount;

	auto lineNumFormat = StrView("{:6} |");

	FmtTo(outStr, lineNumFormat, writeLineNum);

	const Int tabWidth = 4;

	Int writeArrow     = 0;
	Int writeCol       = 0;
	Int writeActualCol = 0; // col after tab to spaces;

	for (CH ch : lastFewLines) {
		if (ch == '\r') continue;

		if (writeCol == _column)
			writeArrow = writeActualCol;

		if (ch == '\t') {
			Int spaces = tabWidth - (writeCol % tabWidth);
			for (Int t = 0; t < spaces; t++) {
				outStr << ' ';
			}
			writeCol++;
			writeActualCol += spaces;
			continue;
		}

		outStr << ch;
		writeCol++;
		writeActualCol++;

		if (ch == '\n') {
			writeCol = 0;
			writeActualCol = 0;
			writeLineNum++;
			FmtTo(outStr, lineNumFormat, writeLineNum);
			continue;
		}
	}

	outStr << '\n';
	FmtTo(outStr, lineNumFormat, ' ');

	for (Int i = 0; i < writeArrow; i++) {
		outStr << ' ';
	}
	outStr.appendUtf("^----\n");

	FmtTo(outStr, "{}:({},{})", _filename, _lineNumber, _column);
}

//-----------

#define	E(T)	\
	/*---- The explicit instantiation ---*/ \
	template class LexerSource<T>; \
//--------
	AX_TYPE_LIST_CHAR(E)
#undef	E

} // namespace