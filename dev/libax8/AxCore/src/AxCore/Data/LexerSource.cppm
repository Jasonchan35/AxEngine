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

	StrView_<CH> str() const { return _source.sliceFrom(_pos); }
	
	bool trim(StrView_<CH> s);
	void trimSpaceAndTab();

	void setFilename(StrView filename) { _filename = filename; }
	StrView filename() const	{ return _filename; }

	Int		lineNumber() const	{ return _lineNumber; }
	Int		column() const		{ return _column; }
	Int		pos() const			{ return _pos; }
	Int		remain() const		{ return _source.size() - _pos; }

	TempString location() const	{ return Fmt("file:{}:{}", _filename, _lineNumber); }
	void appendSourceLocation(IString_<CH>& outStr, Int dumpLineCount);

	StrView_<CH> getLastFewLines(Int n, Int* outCount);

private:
	StrView		_filename;
	Int			_lineNumber = 1;
	Int			_column = 1;

	Int				_pos = 0;
	StrView_<CH>	_source;

	CH			_ch = 0;
};

template<class CH> inline
CH LexerSource<CH>::nextChar() {
	if (!_source.inBound(_pos)) {
		_ch = 0;
		return 0;
	}
	_ch = _source[_pos];
	_pos++;

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
void LexerSource<CH>::trimSpaceAndTab() {
	if (_ch == ' ' or _ch == '\t') {
		nextChar();
	}
}


} // namespace
