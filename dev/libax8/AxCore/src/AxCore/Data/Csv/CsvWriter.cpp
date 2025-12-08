module;
#include "AxCore-pch.h"
module AxCore.CsvWriter;

namespace ax {

template<class CH>
void CsvWriter_<CH>::writeCell(StrView_<CH> str) {
	if (_col > 0) _out.appendUtf(_seperator);

	_out.appendUtf('\"');

	for (auto& ch : str) {
		if (ch == '\"') {
			_out.appendUtf("\"\"");
		} else {
			_out.appendUtf(ch);
		}
	}

	_out.appendUtf(L'\"');
	_col++;
}

template<class CH>
void CsvWriter_<CH>::writeNewline() {
	_out.appendUtf("\n");
	_col = 0;
	_line++;
}

#define	E(T)	\
	/*---- The explicit instantiation ---*/ \
	template class CsvWriter_<T>; \
//--------
	AX_TYPE_LIST_CHAR(E)
#undef	E

} // namespace ax