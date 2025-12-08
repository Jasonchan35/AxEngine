module;
#include "AxCore-pch.h"
module AxCore.LexerSource;

namespace ax {

template<class CH>
void LexerSource<CH>::reset() {
	_source		= {};
	_filename	= {};
	_pos		= 0;
	_lineNumber = 1;
	_column		= 1;
}

template<class CH>
void LexerSource<CH>::init(StrView_<CH> source, StrView filename) {
	reset();
	_filename = filename;
	_source	  = source;
	_pos	  = 0;
}

template<class CH>
StrView_<CH> LexerSource<CH>::getLastFewLines(Int n, Int* outCount) {
	if (!_source.inBound(_pos)) return StrView_<CH>();

	Int p = _pos;

	Int found = 0;
	while (_source.inBound(p) && found < n) {
		if (_source[p] == '\n')
			found++;
		if (p <= 0) break;
		p--;
	}

	if (_source.inBound(p) && _source[p] == '\n')
		p++;

	if (outCount) *outCount = found;

// find end of this line
	Int e = _pos;
	while (_source.inBound(e) && _source[e] != '\n')
		e++;
	if (!_source.inBound(e)) e--;

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