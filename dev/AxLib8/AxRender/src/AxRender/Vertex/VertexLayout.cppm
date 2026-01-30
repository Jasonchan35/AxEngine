module;
export module AxRender:VertexLayout;
export import :RenderDataType;

export namespace ax /*::AxRender*/ {

enum class VertexSemantic : u16;

#define AX_VertexIndexType_ENUM_LIST(E) \
	E(None,) \
	E(u16,) \
	E(u32,) \
//----
AX_ENUM_CLASS(AX_VertexIndexType_ENUM_LIST, VertexIndexType, u8)

#define AX_VertexLayoutNormalType_ENUM_LIST(E) \
	E(None,) \
	E(Normal,) \
	E(Binormal,) \
	E(Tangent,) \
//----
AX_ENUM_CLASS(AX_VertexLayoutNormalType_ENUM_LIST, VertexNormalCount, u8)


template<class T> struct VertexIndexType_get_Struct;
template<> struct VertexIndexType_get_Struct<u16> { static constexpr VertexIndexType value = VertexIndexType::u16; };
template<> struct VertexIndexType_get_Struct<u32> { static constexpr VertexIndexType value = VertexIndexType::u32; };
template<class T> constexpr VertexIndexType VertexIndexType_get = VertexIndexType_get_Struct<T>::value;


inline constexpr Int VertexIndexType_stride(VertexIndexType t) {
	switch (t) {
		case VertexIndexType::u16: return AX_SIZEOF(u16);
		case VertexIndexType::u32: return AX_SIZEOF(u32);
		default: return 0;
	}
}

inline
Int VertexIndexType_sizeInBytes(VertexIndexType t) {
	switch (t) {
		case VertexIndexType::None:	return 0;
		case VertexIndexType::u16:	return AX_SIZEOF(u16);
		case VertexIndexType::u32:	return AX_SIZEOF(u32);
		default: AX_ASSERT(false);	return 0;
	}
}

inline
Int VertexIndexType_limit(VertexIndexType t) {
	switch (t) {
		case VertexIndexType::None:	return NumLimit<Int>::max();
		case VertexIndexType::u16:	return NumLimit<u16>::max();
		case VertexIndexType::u32:	return NumLimit<u32>::max();
		default: AX_ASSERT(false);	return 0;
	}
}

using VertexSemanticIndex = u8;

#define VertexSemanticType_ENUM_LIST(E) \
	E(None, )                           \
	E(POSITION, )                       \
	E(COLOR, )                          \
	E(TEXCOORD, )                       \
	E(NORMAL, )                         \
	E(TANGENT, )                        \
	E(BINORMAL, )                       \
                                        \
	E(SV_POSITION, )                    \
	E(SV_DEPTH, )                       \
	E(SV_COVERAGE, )                    \
	E(SV_VERTEXID, )                    \
	E(SV_PRIMITIVEID, )                 \
	E(SV_INSTANCEID, )                  \
	E(SV_DISPATCHTHREADID, )            \
	E(SV_GROUPID, )                     \
	E(SV_GROUPINDEX, )                  \
	E(SV_GROUPTHREADID, )               \
	E(SV_GSINSTANCEID, )                \
//----
AX_ENUM_CLASS(VertexSemanticType_ENUM_LIST, VertexSemanticType, u8)

struct VertexSemanticUtil {
	using Semantic	= VertexSemantic;
	using Type		= VertexSemanticType;
	using Index		= VertexSemanticIndex;

	static constexpr Semantic make(Type type, Index index) { return static_cast<Semantic>((ax_enum_int(type) << 8) | index); }
	static constexpr u16	make16(Type type, Index index) { return static_cast<u16>(make(type, index)); }
	static constexpr Type	getType (Semantic v) { return static_cast<Type>(ax_enum_int(v) >> 8); }
	static constexpr Index	getIndex(Semantic v) { return static_cast<u8  >(ax_enum_int(v)); }
};

#define VertexSemantic_ENUM_LIST(E) \
	E(None,				= 0) \
	\
	E(SV_POSITION,			= VertexSemanticUtil::make16(VertexSemanticType::SV_POSITION,			0)) \
	E(SV_DEPTH,				= VertexSemanticUtil::make16(VertexSemanticType::SV_DEPTH,				0)) \
	E(SV_COVERAGE,			= VertexSemanticUtil::make16(VertexSemanticType::SV_COVERAGE,			0)) \
	E(SV_VERTEXID,			= VertexSemanticUtil::make16(VertexSemanticType::SV_VERTEXID,			0)) \
	E(SV_PRIMITIVEID,		= VertexSemanticUtil::make16(VertexSemanticType::SV_PRIMITIVEID,		0)) \
	E(SV_INSTANCEID,		= VertexSemanticUtil::make16(VertexSemanticType::SV_INSTANCEID,			0)) \
	E(SV_DISPATCHTHREADID,	= VertexSemanticUtil::make16(VertexSemanticType::SV_DISPATCHTHREADID,	0)) \
	E(SV_GROUPID,			= VertexSemanticUtil::make16(VertexSemanticType::SV_GROUPID,			0)) \
	E(SV_GROUPINDEX,		= VertexSemanticUtil::make16(VertexSemanticType::SV_GROUPINDEX,			0)) \
	E(SV_GROUPTHREADID,		= VertexSemanticUtil::make16(VertexSemanticType::SV_GROUPTHREADID,		0)) \
	E(SV_GSINSTANCEID,		= VertexSemanticUtil::make16(VertexSemanticType::SV_GSINSTANCEID,		0)) \
	\
	E(POSITION,				= VertexSemanticUtil::make16(VertexSemanticType::POSITION, 0)) \
	\
	E(TEXCOORD0,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 0)) \
	E(TEXCOORD1,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 1)) \
	E(TEXCOORD2,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 2)) \
	E(TEXCOORD3,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 3)) \
	E(TEXCOORD4,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 4)) \
	E(TEXCOORD5,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 5)) \
	E(TEXCOORD6,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 6)) \
	E(TEXCOORD7,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 7)) \
	E(TEXCOORD8,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 8)) \
	E(TEXCOORD9,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 9)) \
	\
	E(TEXCOORD10,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 10)) \
	E(TEXCOORD11,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 11)) \
	E(TEXCOORD12,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 12)) \
	E(TEXCOORD13,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 13)) \
	E(TEXCOORD14,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 14)) \
	E(TEXCOORD15,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 15)) \
	E(TEXCOORD16,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 16)) \
	E(TEXCOORD17,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 17)) \
	E(TEXCOORD18,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 18)) \
	E(TEXCOORD19,			= VertexSemanticUtil::make16(VertexSemanticType::TEXCOORD, 19)) \
	\
	E(COLOR0,				= VertexSemanticUtil::make16(VertexSemanticType::COLOR,    0)) \
	E(COLOR1,				= VertexSemanticUtil::make16(VertexSemanticType::COLOR,    1)) \
	E(COLOR2,				= VertexSemanticUtil::make16(VertexSemanticType::COLOR,    2)) \
	E(COLOR3,				= VertexSemanticUtil::make16(VertexSemanticType::COLOR,    3)) \
	E(COLOR4,				= VertexSemanticUtil::make16(VertexSemanticType::COLOR,    4)) \
	E(COLOR5,				= VertexSemanticUtil::make16(VertexSemanticType::COLOR,    5)) \
	E(COLOR6,				= VertexSemanticUtil::make16(VertexSemanticType::COLOR,    6)) \
	E(COLOR7,				= VertexSemanticUtil::make16(VertexSemanticType::COLOR,    7)) \
	\
	E(NORMAL0,				= VertexSemanticUtil::make16(VertexSemanticType::NORMAL,   0)) \
	E(NORMAL1,				= VertexSemanticUtil::make16(VertexSemanticType::NORMAL,   1)) \
	E(NORMAL2,				= VertexSemanticUtil::make16(VertexSemanticType::NORMAL,   2)) \
	E(NORMAL3,				= VertexSemanticUtil::make16(VertexSemanticType::NORMAL,   3)) \
	E(NORMAL4,				= VertexSemanticUtil::make16(VertexSemanticType::NORMAL,   4)) \
	E(NORMAL5,				= VertexSemanticUtil::make16(VertexSemanticType::NORMAL,   5)) \
	E(NORMAL6,				= VertexSemanticUtil::make16(VertexSemanticType::NORMAL,   6)) \
	E(NORMAL7,				= VertexSemanticUtil::make16(VertexSemanticType::NORMAL,   7)) \
	\
	E(TANGENT0,				= VertexSemanticUtil::make16(VertexSemanticType::TANGENT,  0)) \
	E(TANGENT1,				= VertexSemanticUtil::make16(VertexSemanticType::TANGENT,  1)) \
	E(TANGENT2,				= VertexSemanticUtil::make16(VertexSemanticType::TANGENT,  2)) \
	E(TANGENT3,				= VertexSemanticUtil::make16(VertexSemanticType::TANGENT,  3)) \
	E(TANGENT4,				= VertexSemanticUtil::make16(VertexSemanticType::TANGENT,  4)) \
	E(TANGENT5,				= VertexSemanticUtil::make16(VertexSemanticType::TANGENT,  5)) \
	E(TANGENT6,				= VertexSemanticUtil::make16(VertexSemanticType::TANGENT,  6)) \
	E(TANGENT7,				= VertexSemanticUtil::make16(VertexSemanticType::TANGENT,  7)) \
	\
	E(BINORMAL0,			= VertexSemanticUtil::make16(VertexSemanticType::BINORMAL, 0)) \
	E(BINORMAL1,			= VertexSemanticUtil::make16(VertexSemanticType::BINORMAL, 1)) \
	E(BINORMAL2,			= VertexSemanticUtil::make16(VertexSemanticType::BINORMAL, 2)) \
	E(BINORMAL3,			= VertexSemanticUtil::make16(VertexSemanticType::BINORMAL, 3)) \
	E(BINORMAL4,			= VertexSemanticUtil::make16(VertexSemanticType::BINORMAL, 4)) \
	E(BINORMAL5,			= VertexSemanticUtil::make16(VertexSemanticType::BINORMAL, 5)) \
	E(BINORMAL6,			= VertexSemanticUtil::make16(VertexSemanticType::BINORMAL, 6)) \
	E(BINORMAL7,			= VertexSemanticUtil::make16(VertexSemanticType::BINORMAL, 7)) \
//-----
AX_ENUM_CLASS(VertexSemantic_ENUM_LIST, VertexSemantic, u16)

struct VertexLayoutElement {
	VertexSemantic semantic = VertexSemantic::None;
	u16            offset   = 0;
	RenderDataType dataType = RenderDataType::None;

	VertexSemanticType SemanticType() const { return VertexSemanticUtil::getType(semantic); }
		
	bool operator==(const VertexLayoutElement& r) const = default; 
};

struct VertexLayoutDesc {
	using Element = VertexLayoutElement;

	Int	strideInBytes = 0;
	Array<Element, 16>	elements;

	bool operator==(const VertexLayoutDesc& r) const {
		return strideInBytes == r.strideInBytes && elements == r.elements;
	}

	HashInt onHashInt() const { return HashInt::s_make(elements.toByteSpan()); }

	template<class VERTEX, class ATTR>
	void addElement(VertexSemantic semantic, ATTR VERTEX::*attr) {
		if constexpr (Type_IsFixedArray<ATTR>)  {
			u16 n = static_cast<u16>(ATTR::kSize);
			for (u16 i = 0; i < n; i++) {
				_addElement(semantic + static_cast<VertexSemantic>(i), attr, i);
			}

		} else if constexpr (std::is_array_v<ATTR>) { // C Array
			u16 n = static_cast<u16>(std::extent_v<ATTR>);
			for (u16 i = 0; i < n; i++) {
				_addElement(semantic + static_cast<VertexSemantic>(i), attr, i);
			}
		} else {
			_addElement(semantic, attr, 0);
		}
	}

	const Element* find(VertexSemantic semantic) const {
		for (auto& e : elements) {
			if (e.semantic == semantic) return &e;
		}
		return nullptr;
	}

	bool hasSemantic(VertexSemantic t) const {
		for (auto& e : elements) {
			if (e.semantic == t) return true;
		}
		return false;
	}
	
	bool hasSemanticType(VertexSemanticType t) const {
		for (auto& e : elements) {
			if (e.SemanticType() == t) return true;
		}
		return false;
	}
	
	bool hasPosition() const { return hasSemantic(VertexSemantic::POSITION); }
	bool hasNormal0	() const { return hasSemantic(VertexSemantic::NORMAL0 ); }
	
private:
	template<class VERTEX, class ATTR>
	void _addElement(VertexSemantic semantic, ATTR VERTEX::*attr, u16 index) {
		if constexpr (Type_IsFixedArray<ATTR>) {
			using A = typename ATTR::Element;
			auto& dst = elements.emplaceBack();
			dst.semantic = semantic;
			dst.offset   = static_cast<u16>(MemUtil::memberOffset(attr) + AX_SIZEOF(A) * index);
			dst.dataType = RenderDataType_get<A>;

		} else {
			using A      = std::remove_extent_t<ATTR>;
			auto& dst    = elements.emplaceBack();
			dst.semantic = semantic;
			dst.offset   = static_cast<u16>(MemUtil::memberOffset(attr) + AX_SIZEOF(A) * index);
			dst.dataType = RenderDataType_get<A>;
		}
		strideInBytes = AX_SIZEOF(VERTEX);
	}
};

class VertexLayout {
public:
	VertexLayout() = default;
	VertexLayout(const VertexLayoutDesc* desc) : _desc(desc) {}

	const VertexLayoutDesc* operator->() const { return _desc; }

	void operator=(const VertexLayoutDesc* desc) { _desc = desc; }

	bool operator==(const VertexLayout& r) const { return _desc == r._desc; }
	bool operator!=(const VertexLayout& r) const { return _desc != r._desc; }

	explicit operator bool() const { return _desc != nullptr; }

	using NormalCount = VertexNormalCount;
	static VertexLayout s_make(Int colorCount, Int uvCount, NormalCount normalCount);
	
protected:
	const VertexLayoutDesc* _desc = nullptr;
};

class VertexLayoutManager {
public:
	VertexLayoutManager();

	static VertexLayoutManager* s_instance();
	VertexLayout registerLayout(const VertexLayoutDesc& desc);

private:
	struct MData : public NonCopyable {
		Dict<VertexLayoutDesc, VertexLayout>	table;
	};
	LockProtected<Mutex, MData>	_md;
};


} // namespace