module;


export module AxCore.NameId;
export import AxCore.PersistString;
export import AxCore.Formatter;

export namespace ax {

template<class CH, class ID>
class NameId_ { // copyable
	using This	= NameId_;
public:
	using Id	= ID;
	using PStr	= PersistString_<CH>;
	using View  = StrView_<CH>;

	NameId_() = default;
	NameId_(const NameId_&) = default;
	NameId_(const PStr& name_, const Id& id_) : _name(name_), _id(id_) {}

	static NameId_ s_make(StrView_<CH> str);

	constexpr StrView	name() const { return _name; }
	constexpr Id		id() const   { return _id; }

	AX_NODISCARD bool operator==(const This& r) const { return _name == r._name && _id == r._id; }
	AX_NODISCARD bool operator!=(const This& r) const { return !operator==(); }

	explicit operator bool() const { return _name || _id >= 0; }

	template<class FMT_CH>
	void onFormat(Format_<FMT_CH> & f) const {
		f << _name;
		if (_id >= 0) f << _id;
	}

	TempString_<CH> toString() { return Fmt("{}", *this); }

	AX_NODISCARD bool hasNoId() const { return _id == kNoId; }

	AX_NODISCARD HashInt onHashInt() const { return HashInt::s_make(_name) ^ HashInt::s_make(_id); }
	static constexpr Id kNoId = -1;
private:
	PStr _name;
	Id   _id = kNoId;
};

template<class CH, class ID>
class InNameId_ : public NameId_<CH, ID> {
	using Base = NameId_<CH, ID>;
public:
	InNameId_(Base r) : Base(r) {}
	template<class R> constexpr InNameId_(const R& r) : Base(Base::s_make(r)) {}
};

using NameId   = NameId_<Char, Int>;
using InNameId = InNameId_<Char, Int>;

template<class CH, class ID> inline
auto NameId_<CH, ID>::s_make(StrView_<CH> str) -> NameId_ {
	if (!str) return NameId_();

	auto* s = str.begin();
	auto* e = str.end();
	auto* p = e - 1;
	for (; p >= s; --p) {
		if (!CharUtil::isDigit(*p)) {
			++p;
			break;
		}
	}

	ID outId = kNoId;
	if (p != e) {
		auto len = e - p;
		if (!StrView(p, len).tryParse(outId)) { throw Error_ParseString(); }
	}

	return NameId_(PStr::s_make(StrView_<CH>(s, p - s)), outId);
}

template <class... ARGS>
constexpr NameId FmtName(FormatString_<Char, ARGS...>&& fmt, const ARGS&... args) {
	return NameId::s_make(Fmt(AX_FORWARD(fmt), AX_FORWARD(args)...));
}


} // namespace