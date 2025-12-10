module;


export module AxCore.LexerSource;
export import AxCore.Logger;

export namespace ax {

template<class CH>
class LexerSource : public NonCopyable {
public:
	LexerSource() { reset(); }

	void reset();
	void init(StrView_<CH> source, StrView filename);

	CH ch() const { return _ch; }
	CH nextChar();

	void setFilename(StrView filename) { _filename = filename; }
	StrView filename() const	{ return _filename; }

	Int		lineNumber() const	{ return _lineNumber; }
	Int		column() const		{ return _column; }
	Int		pos() const			{ return _pos; }

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


} // namespace
