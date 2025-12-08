module;
#include "AxCore-pch.h"

export module AxCore.CsvReader;
export import AxCore.Logger;

export namespace ax {


template<class CH>
class CsvReader_ {
public:
	using Cell = String_<CH>;

	class Row {
	public:
		Array<Cell, 64>	cells;

		template<class FMT_CH>
		void onFmt(Format_<FMT_CH>& ctx) const {
			ctx << cells;
		}
	};

	CsvReader_(StrView_<CH> data, CH separator, StrView_<CH> filenameForErrorMessage = StrView_<CH>());

	bool nextRow();

	StrView_<CH>	cell(Int index) { return _row.cells.inBound(index) ? StrView_<CH>(_row.cells[index]) : StrView_<CH>(); }
	const Row&		row() const { return _row; }

private:

	bool		_readCell(Cell& cell);
	void		_readString(Cell& str);

	StrView_<CH>	_data;
	StrView_<CH>	_filename;
	Int				_pos = 0;
	CH				_separator = 0;
	Row				_row;
};

using CsvReader	  = CsvReader_<Char>;

using CsvReaderA  = CsvReader_<CharA >;
using CsvReaderW  = CsvReader_<CharW >;
using CsvReader16 = CsvReader_<Char16>;
using CsvReader32 = CsvReader_<Char32>;


} // namespace ax

