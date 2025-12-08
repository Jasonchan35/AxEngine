module;
#include "AxCore-pch.h"
module AxCore.CsvReader;

namespace ax {

template<class CH>
CsvReader_<CH>::CsvReader_(StrView_<CH> data, CH separator, StrView_<CH> filenameForErrorMessage) 
	: _data(data)
	, _filename(filenameForErrorMessage)
	, _pos(0)
	, _separator(separator)
{
}

template<class CH>
bool CsvReader_<CH>::nextRow() {
	_row.cells.clear();
	if (!_data.inBound(_pos)) return false;

	for (;;) {
		auto& cell = _row.cells.emplaceBack();
		if (!_readCell(cell))
			break;
	}

	return true;
}

template<class CH>
bool CsvReader_<CH>::_readCell(Cell& cell) {
	if (!_data.inBound(_pos)) return false;

	for ( ; _data.inBound(_pos) ; _pos++) {
		auto c = _data[_pos];
		if (c == '\"') {
			_pos++;
			_readString(cell);
			continue;
		}

		if (c == _separator) {
			_pos++;
			return true;
		}

		if (c == '\r') {
			_pos++;
			if (_data[_pos] == '\n') _pos++;
			return false;
		}

		if (c == '\n') {
			_pos++;
			return false;
		}

		cell.append(c);
	}

	return false;
}

template<class CH>
void CsvReader_<CH>::_readString(Cell& cell) {
	for (;; _pos++) {
		if (!_data.inBound(_pos))
			throw Error_Undefined(); // End of file within string

		auto c = _data[_pos];

		if (c == '\"') {
			return;
		}

		cell.append(c);
	}
}


#define	E(T)	\
	/*---- The explicit instantiation ---*/ \
	template class CsvReader_<T>; \
//--------
	AX_TYPE_LIST_CHAR(E)
#undef	E

} // namespace ax
