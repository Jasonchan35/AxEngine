module;


export module AxCore.LexerSource;
export import AxCore.Logger;
export import AxCore.UtfUtil;

export namespace ax {

template<class CH>
class LexerSource : public NonCopyable {
public:
	LexerSource() { reset(); }

	void reset();
	void init(StrView_<CH> source, StrView filename);

	CH ch() const { return _ch; }
	CH nextChar();
	
	CH advancePos(Int n);

	StrView_<CH> remain() const { return _remain; }
	
	bool startsWith(StrView_<CH> s) const { return remain().startsWith(s); }
	
	bool match(StrView_<CH> s);
	void trimSpaceTab();
	void trimSpaceTab_Newline();
	
	void skipUntil(StrView_<CH> delimiter, bool bAdvancePos);
	
	void read_c_string_literal(IString_<CH>& outStr);
	
	CH readHex(Int n);
	CH readOctal(Int n);

	void setFilename(StrView filename) { _filename = filename; }
	StrView filename() const	{ return _filename; }

	Int		lineNumber() const	{ return _lineNumber; }
	Int		column() const		{ return _column; }
	Int		pos() const			{ return _source.size() - _remain.size(); }

	TempString location() const	{ return Fmt("file:{}:{}", _filename, _lineNumber); }
	void appendSourceLocation(IString_<CH>& outStr, Int dumpLineCount);

	StrView_<CH> getLastFewLines(Int n, Int* outCount);

	CH& chRef() { return _ch; }
	
private:
	StrView      _filename;
	Int          _lineNumber = 1;
	Int          _column     = 1;
	StrView_<CH> _source;
	StrView_<CH> _remain;
	CH			_ch = 0;
	
};

template<class CH> inline
CH LexerSource<CH>::nextChar() {
	if (_remain.size() < 1) {
		_remain = {};
		_ch = 0;
		return 0;
	}
	_remain = _remain.sliceFrom(1);
	_ch = _remain.size() > 0 ? _remain[0] : 0;

	if (_ch == '\n') {
		_lineNumber++;
		_column = 1;
	} else {
		_column++;
	}
	return _ch;
}

template<class CH> inline
CH LexerSource<CH>::advancePos(Int n) {
	for (Int i = 0; i < n; i++) 
		nextChar();
	return _ch;
}

template<class CH> inline
void LexerSource<CH>::trimSpaceTab() {
	while (_ch == ' ' || _ch == '\t') {
		nextChar();
	}
}

template<class CH>
void LexerSource<CH>::trimSpaceTab_Newline() {
	while (_ch == ' ' || _ch == '\t' || _ch == '\r' || _ch == '\n') {
		nextChar();
	}	
}

} // namespace
