module;

export module AxCore.Rtti;
export import AxCore.NameId;
export import AxCore.WPtr;
export import AxCore.MetaType;
export import AxCore.Dict;
export import AxCore.Logger;

export namespace ax {

struct MutRtti;
using Rtti = const MutRtti;
template<class T> Rtti* rttiOf();

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
	// static constexpr Int	kBufSize = 64;
	// char					_buf[kBufSize];
	MetaAttr*				_attr = nullptr;
	String					_name;
};

using RttiAttr = const MutRttiAttr;

#if 0
#pragma mark ---------------- RttiField -------------------
#endif

struct MutRttiField : public NonCopyable {
	Rtti*	fieldOwner = nullptr;
	Rtti*	fieldType = nullptr;
	Int		offset = INT_MAX;
	NameId	name;
};

using RttiField = const MutRttiField;

#if 0
#pragma mark ---------------- Rtti -------------------
#endif

template<class T> struct MutRttiInit_OutOfClass_;
template<class T> struct MutRttiInit_FromMetaType_;

// template<class T> concept CON_HasRttiInfo = requires { { T::s_rtti() } -> std::same_as<Rtti*>; };
template<class T> concept CON_HasRttiInit = requires { typename T::RttiInit; };
template<class T> concept CON_HasMutRttiInit_OutOfClass = requires { MutRttiInit_OutOfClass_<T>(); };

template<class RTTI_INIT> struct RttiInit_Make : public RTTI_INIT {
	RttiInit_Make() {
		auto* base = RTTI_INIT::base;
		if (base) {
			this->allFields.ensureCapacity(base->allFields.size() + this->ownFields.size());
			this->allFields = base->allFields;
		}
		for (auto* field : this->ownFields) {
			this->allFields.emplaceBack(field);
		}
	}
};

template<class T>
struct Rtti_Handler_ {
	static Rtti* s_rtti() {
		static_assert(!std::is_const_v<T>);
		static_assert(!std::is_reference_v<T>);
		static_assert(!std::is_pointer_v<T>);
		if constexpr(CON_HasRttiInit<T>) {
			static RttiInit_Make<typename T::MutRttiInit> s;
			return &s;
		} else if constexpr (CON_HasMutRttiInit_OutOfClass<T>) {
			static RttiInit_Make<MutRttiInit_OutOfClass_<T>> s;
			return &s;
		} else {
			static RttiInit_Make<MutRttiInit_FromMetaType_<T>> s;
			return &s;
		}
	}
};

template<> struct Rtti_Handler_<NoBaseClass> {
	static Rtti* s_rtti() { return nullptr; }
};

template<class T> Rtti* rttiOf() { return Rtti_Handler_<std::remove_cv_t<T>>::s_rtti(); }

struct MutRtti : public NonCopyable {
	template<class R>
	constexpr bool isKindOf() const { return isKindOf(rttiOf<R>()); }
	constexpr bool isKindOf(Rtti* r) const;

	RttiField*	findField(NameId fieldName) const { return ownFieldsDict.find(fieldName); }
	
	void debugDump() const;

	Rtti*	base = nullptr;
	NameId	name;
	NameId	displayName;
	
	void addField(InNameId name_, Rtti* fieldType_, Int offset_) {
		auto& field      = ownFieldsDict.add(name_);
		field.name       = name_;
		field.fieldOwner = this;
		field.fieldType  = fieldType_;
		field.offset     = offset_;
		ownFields.emplaceBack(&field);
	}

	template<class OBJ, class FIELD>
	void addField(InNameId name_, FIELD OBJ::*ptrToField) {
		addField(name_, rttiOf<FIELD>(), MemUtil::memberOffset(ptrToField));
	}

	Dict<NameId, MutRttiField>			ownFieldsDict;
	Array<RttiField*>					ownFields;
	Array<RttiField*>					allFields;

	Dict<RttiAttrClassId, MutRttiAttr>	ownAttrDict;
	Array<RttiAttr*>					ownAttrs;
};

// use for declare out of class
template<class T> struct MutRttiInit_;

template<> struct MutRttiInit_FromMetaType_<NoBaseClass> {}; 

template<class T>
struct MutRttiInit_FromMetaType_ : public MutRtti {
	using ObjThis      = T;
	using ObjBase      = BaseClassOf<T>;

	using MetaType     = MetaTypeOf<T>;
	using BaseMetaType = MetaTypeOf<ObjBase>;

	static constexpr bool noBase = std::is_same_v<ObjBase, NoBaseClass>;
	
	struct OwnField_Handler {
		template<Int Index, class Field>
		static void onEach(MutRttiInit_FromMetaType_* rtti) {
			rtti->addField(Field::s_name(), rttiOf<typename Field::FieldType>(), Field::s_offset());
		}
	};

	MutRttiInit_FromMetaType_() {
//		static_assert(Type_IsBaseOf<IMetaType, MetaType>, "MetaType must based on IMetaType");
		this->base = rttiOf<ObjBase>();
		this->name = NameId::s_make(ax_metatype_get_class_name<T>());
		using OwnFields = typename MetaType::OwnFields;
		this->ownFields.ensureCapacity(OwnFields::kSize);
		OwnFields::template ForEachType<OwnField_Handler>(this);
	}
};

constexpr
bool Rtti::isKindOf(Rtti* r) const {
	if (!r) return false;

	const auto* p = this;
	while (p) {
		if (p == r) return true;
		p = p->base;
	}
	return false;
}

#if 0
#pragma mark ---------------- RttiManager -------------------
#endif

#if 0
#pragma mark ---------------- RttiObject -------------------
#endif

class RttiObject : public WPtr_Referenceable {
	AX_NON_COPYABLE(RttiObject)
public:
	RttiObject() = default;
	virtual ~RttiObject() = default;
	
	AX_META_TYPE(RttiObject, NoBaseClass) {};
	
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
		if (!dst) {
			dst = rttiCast<DST>(src);
		}
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

