module AxNinjaBuild;

namespace ax {

TempString AxNinjaBuild::escapeString(StrView str) {
	TempString o;
	constexpr auto charNeedEscape = StrView(" \n:");
	
	for (auto& ch : str) {
		if (charNeedEscape.find(ch)){
			o << '$';
		}
		o << ch;
	}

	return o;
}

} // namespace


