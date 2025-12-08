module;
#include "AxCore-pch.h"

export module AxCore.CsvWriter;
export import AxCore.Logger;

export namespace ax {

template<class CH>
class CsvWriter_ {
public:
	CsvWriter_(IString_<CH>& outStr, CH separator)
	: _out(outStr), _seperator(separator) {}

	void writeCell(StrView_<CH> str);
	void writeNewline();

private:
	Int				_col  = 0;
	Int				_line = 0;
	IString_<CH>&	_out;
	CH				_seperator;
};

using CsvWriter = CsvWriter_<Char>;

using CsvWriterA  = CsvWriter_<CharA >;
using CsvWriterW  = CsvWriter_<CharW >;
using CsvWriter16 = CsvWriter_<Char16>;
using CsvWriter32 = CsvWriter_<Char32>;

} // namespace ax

