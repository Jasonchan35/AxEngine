module;

#include "Rtti_MACRO.h"

export module AxCore.Rtti;
export import AxCore.NameId;
export import AxCore.WPtr;
export import AxCore.MetaType;
export import AxCore.Dict;

export namespace ax {

struct Rtti;
template<class T> struct Rtti_;

#if 0
#pragma mark ---------------- RttiAttr -------------------
#endif

enum class RttiAttrClassIdParam {};
template<class ATTR> void ax_rtti_attr_class_id_func(RttiAttrClassIdParam) {};
using RttiAttrClassId = void (*)(RttiAttrClassIdParam);

template<class ATTR>
RttiAttrClassId ax_rtti_attr_class_id() { return &ax_rtti_attr_class_id_func<typename ATTR::AttrType>; }

class MutRttiAttr : public NonCopyable {
public:
	template<class CH>
	void onFormat(Format_<CH>& ctx) const {
		ctx << "attr: " << _name;
	}

	StrView	name() const { return _name; }

	template<class ATTR> void init();
	template<class ATTR> const ATTR* getAttr() const;

	RttiAttrClassId	classId() const { return _classId; }

private:
	RttiAttrClassId			_classId = nullptr;
	static constexpr Int	kBufSize = 64;
	char					_buf[kBufSize];
	MetaAttr*				_attr = nullptr;
	String					_name;
};

using RttiAttr = const MutRttiAttr;

#if 0
#pragma mark ---------------- RttiField -------------------
#endif

struct MutRttiField : public NonCopyable {
	NameId	name() const { return _name; }
	Rtti*	fieldType() const { return _fieldType; }
	Rtti*	fieldOwner() const { return _fieldOwner; }

	template<class R> friend struct Rtti_;
	friend struct Rtti;
protected:
	NameId	_name;
	Rtti*	_fieldType = nullptr;
	Rtti*	_fieldOwner = nullptr;
};

using RttiField = const MutRttiField;

#if 0
#pragma mark ---------------- Rtti -------------------
#endif

template<class T> struct Rtti_Handler_ {
	static Rtti* s_rtti() {
		static Rtti_<T> s;
		return &s;
	}
};

template<> struct Rtti_Handler_<NoBaseClass> {
	static Rtti* s_rtti() { return nullptr; }
};

template<class T> Rtti* rttiOf() { return Rtti_Handler_<T>::s_rtti(); }

struct Rtti : public NonCopyable {
	template<class R>
	constexpr bool isKindOf() const { return isKindOf(rttiOf<R>()); }
	constexpr bool isKindOf(Rtti* r) const;

	Rtti*		base() const		{ return _base; }
	NameId		name() const		{ return _name; }
	NameId		displayName() const	{ return _displayName; }
	
	RttiField*	getOwnField(Int index) const;
	RttiField*	getField(Int index) const;

	Span<RttiField*>	ownFields() const { return _ownFields; }
	Span<RttiField*>	allFields() const { return _allFields; }

	RttiField*	findField(NameId name) const { return _ownFieldsDict.find(name); }	
	
	void DebugDump();

	template<class R> friend struct Rtti_; 
protected:
	Rtti*	_base = nullptr;
	NameId	_name;
	NameId	_displayName;

	Dict<NameId, MutRttiField>			_ownFieldsDict;
	Array<RttiField*>					_ownFields;

	Dict<NameId, MutRttiField>			_allFieldsDict;
	Array<RttiField*>					_allFields;

	Dict<RttiAttrClassId, MutRttiAttr>	_ownAttrDict;
	Array<RttiAttr*>					_ownAttrs;
};


template<>
struct Rtti_<NoBaseClass> {}; 

template<class T>
struct Rtti_ : public Rtti {
	Rtti_() { _ctor(); }
public:
	using ObjThis      = T;
	using ObjBase      = BaseClassOf<T>;

	using MetaType     = MetaTypeOf<T>;
	using BaseMetaType = MetaTypeOf<ObjBase>;

	static constexpr bool noBase = std::is_same_v<ObjBase, NoBaseClass>;
	
	struct OwnField_Handler {
		template<Int Index, class Field>
		static void onEach(Rtti_* rtti) {
			static MutRttiField field;
			field._name = Field::s_name();
			using FieldType = typename Field::FieldType;
			field._fieldOwner = rtti;
			field._fieldType = rttiOf<FieldType>();
			rtti->_ownFields.emplaceBack(&field);
		}
	};
	
private:
	void _ctor() {
//		static_assert(Type_IsBaseOf<IMetaType, MetaType>, "MetaType must based on IMetaType");

		_base = rttiOf<ObjBase>();
		
		this->_name = MetaType::s_name();
		using OwnFields = typename MetaType::OwnFields;
		
		this->_ownFields.reserve(OwnFields::Size);
		OwnFields::template ForEachType<OwnField_Handler>(this);
		
		//---- All Fields -----
		if (_base) {
			this->_allFields.reserve(_base->_allFields.size() + OwnFields::Size);
			this->_allFields = _base->_allFields;
		}
		for (auto* field : this->_ownFields) {
			this->_allFields.emplaceBack(field);
		}
	}
};

constexpr
bool Rtti::isKindOf(Rtti* r) const {
	if (!r) return false;

	const auto* p = this;
	while (p) {
		if (p == r) return true;
		p = p->_base;
	}
	return false;
}

#if 0
#pragma mark ---------------- RttiObject -------------------
#endif

class RttiObject : public WPtrReferenable {
	AX_NON_COPYABLE(RttiObject)
	AX_TYPE_INFO(RttiObject, NoBaseClass)
public:
	RttiObject() = default;
	virtual ~RttiObject() = default;
	
	using _TYPE_INFO_Base = NoBaseClass;
	struct MetaTypeInit : AX_META_TYPE() {};
	
	static  Rtti* s_rtti ()		{ return rttiOf<This>(); }
	virtual Rtti* rtti() const 	{ return rttiOf<This>(); }
};


template<class DST, class SRC> AX_INLINE
DST* _rttiCast(SRC* src) {
	static_assert(std::is_base_of_v<SRC, DST>);

	if constexpr (std::is_same_v<DST, SRC>) return src;
	if (!src) return nullptr;

	if (auto* srcType = src->rtti()) {
		if (srcType->template isKindOf<DST>())
			return static_cast<DST*>(src);
	}
	return nullptr;
}

template<class DST, class SRC> AX_INLINE       DST* rttiCast(      SRC* src) { return _rttiCast<DST, SRC>(src); }
template<class DST, class SRC> AX_INLINE const DST* rttiCast(const SRC* src) { return _rttiCast<DST, SRC>(ax_const_cast(src)); }

template<class DST, class SRC> AX_INLINE
UPtr<DST> rttiCast(UPtr<SRC> && src) {
	auto dst = UPtr<DST>::s_ref(_rttiCast<DST>(src.ptr()));
	if (dst) src.detach(); // transfer ownership
	return dst;
}

template<class DST, class SRC> AX_INLINE
DST* _rttiCastCheck(SRC* src) {
	if (!src) return nullptr;

	if constexpr (std::is_same_v<DST, SRC>) {
		return src;
	} else {
#if _DEBUG
		auto* dst = rttiCast<DST>(src); 
		AX_ASSERT(dst);
		return dst;
#else
		return static_cast<DST*>(src);
#endif
	}
}

template<class DST, class SRC> AX_INLINE        DST* rttiCastCheck(      SRC* src) { return _rttiCastCheck<DST, SRC>(src); }
template<class DST, class SRC> AX_INLINE  const DST* rttiCastCheck(const SRC* src) { return _rttiCastCheck<DST, SRC>(ax_const_cast(src)); }

} // namespace

