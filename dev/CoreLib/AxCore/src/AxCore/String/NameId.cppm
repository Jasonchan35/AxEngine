module;
#include "AxCore-pch.h"

export module AxCore.NameId;
export import AxCore.PersistString;

export namespace ax {

template<class CH, class ID>
class NameId_ { // copyable
	using This	= NameId_;
public:
	using Id	= ID;
	using Str	= PersistString_<CH>;
	using View  = StrView_<CH>;

	NameId_() = default;

	AX_NODISCARD View	name() const	{ return _name; }
	AX_NODISCARD Id		id() const		{ return _id;   }

	AX_NODISCARD Str	persistName()	{ return _name; }

	AX_NODISCARD bool operator==(const This& r) const { return _name == r._name && _id == r._id; }
	AX_NODISCARD bool operator!=(const This& r) const { return !operator==(); }

	explicit operator bool() const { return _name || _id >= 0; }

	template<class FMT_CH>
	void onFormat(Format_<FMT_CH> & f) const {
		f << _name;
		if (_id >= 0) f << _id;
	}

	AX_NODISCARD bool hasNoId() const { return _id == kNoId; }

	AX_NODISCARD HashInt onHashInt() const { return HashInt_get(_name) ^ HashInt_get(_id); }
	static constexpr Id kNoId = -1;

	static NameId_ s_compute(StrView_<CH> str);
private:
	NameId_(const Str& name, const Id& id) : _name(name), _id(id) {}

	Str	_name;
	Id	_id = kNoId;
};

using NameId = NameId_<Char, Int>;

template<class CH, class ID> inline
auto NameId_<CH, ID>::s_compute(StrView_<CH> str) -> This {
	if (!str) return This();

	auto* s = str.begin();
	auto* e = str.end();
	auto* p = e - 1;
	for (; p >= s; --p) {
		if (!std::isdigit(*p)) {
			++p;
			break;
		}
	}

	ID outId = kNoId;
	if (p != e) {
		auto len = e - p;
		if (!StrView(p, len).tryParse(outId)) { throw Error_ParseString(); }
	}

	return This(Str::s_make(StrView_<CH>(s, p - s)), outId);
}

} // namespace