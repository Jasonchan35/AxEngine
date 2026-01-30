module;
export module AxRender:Vertex;
export import :RenderDataType;
export import :VertexLayout;

export namespace ax /*::AxRender*/ {
struct VertexBase {
	using PosType    = void;
	using ColorType  = void;
	using UvType     = void;
	using NormalType = void;

	static constexpr RenderDataType	kPosType    = RenderDataType::None;
	static constexpr RenderDataType	kColorType  = RenderDataType::None;
	static constexpr RenderDataType	kUvType     = RenderDataType::None;
	static constexpr RenderDataType	kNormalType = RenderDataType::None;

	static constexpr u8 kColorCount	= 0;
	static constexpr u8 kUvCount		= 0;
	static constexpr u8 kNormalCount	= 0;
	static constexpr u8 kTangentCount	= 0;
	static constexpr u8 kBinormalCount	= 0;

	static void onCreateLayout(VertexLayoutDesc* desc) {}
protected:
	VertexBase() = default;
};

template<class VERTEX>
struct VertexLayoutCreator {
	VertexLayoutCreator() {
		VertexLayoutDesc	desc;
		VERTEX::onCreateLayout(&desc);
		layout = VertexLayoutManager::s_instance()->registerLayout(desc);
	}
	VertexLayout	layout = nullptr;
};

struct Vertex_None : public VertexBase {
	static VertexLayout s_layout() {
		static VertexLayoutCreator<Vertex_None> s;
		return s.layout;
	}
};

template<class POS_TYPE>
struct VertexT_Pos : public VertexBase {
	using PosType = POS_TYPE;
	POS_TYPE pos;

	static const RenderDataType	kPosType = RenderDataType_get<POS_TYPE>;

	static VertexLayout s_layout() {
		static VertexLayoutCreator<VertexT_Pos> s;
		return s.layout;
	}

	static void onCreateLayout(VertexLayoutDesc* desc) {
		desc->addElement(VertexSemantic::POSITION, &VertexT_Pos::pos);
	}
};

template<class COLOR_TYPE, u8 COLOR_COUNT, class BASE>
struct VertexT_Color : public BASE {
	using ColorType = COLOR_TYPE;
	FixedArray<COLOR_TYPE, COLOR_COUNT>	color;

	static const RenderDataType	kColorType = RenderDataType_get<COLOR_TYPE>;
	static const u8 kColorCount = COLOR_COUNT;

	static VertexLayout s_layout() {
		static VertexLayoutCreator<VertexT_Color> s;
		return s.layout;
	}

	static void onCreateLayout(VertexLayoutDesc* desc) {
		BASE::onCreateLayout(desc);
		desc->addElement(VertexSemantic::COLOR0, &VertexT_Color::color);
	}
};

template<class UV_TYPE, u8 UV_COUNT, class BASE>
struct VertexT_Uv : public BASE
{
	using UvType = UV_TYPE;
	FixedArray<UV_TYPE, UV_COUNT>	uv;

	static const RenderDataType	kUvType = RenderDataType_get<UV_TYPE>;
	static const u8 kUvCount = UV_COUNT;

	static VertexLayout s_layout() {
		static VertexLayoutCreator<VertexT_Uv> s;
		return s.layout;
	}

	static void onCreateLayout(VertexLayoutDesc* desc) {
		BASE::onCreateLayout(desc);
		desc->addElement(VertexSemantic::TEXCOORD0, &VertexT_Uv::uv);
	}
};

template<class NORMAL_TYPE, u8 NORMAL_COUNT, class BASE>
struct VertexT_Normal : public BASE {
	using NormalType = NORMAL_TYPE;
	FixedArray<NORMAL_TYPE, NORMAL_COUNT>	normal;

	static const RenderDataType	kNormalType = RenderDataType_get<NORMAL_TYPE>;
	static const u8 kNormalCount = NORMAL_COUNT;

	static VertexLayout s_layout() {
		static VertexLayoutCreator<VertexT_Normal> s;
		return s.layout;
	}

	static void onCreateLayout(VertexLayoutDesc* desc) {
		BASE::onCreateLayout(desc);
		desc->addElement(VertexSemantic::NORMAL0, &VertexT_Normal::normal);
	}
};

template<class BINORMAL_TYPE, u8 BINORMAL_COUNT, class BASE>
struct VertexT_Binormal : public BASE
{
	using BinormalType = BINORMAL_TYPE;
	FixedArray<BINORMAL_TYPE, BINORMAL_COUNT>	binormal;

	static const RenderDataType	kBinormalType = RenderDataType_get<BINORMAL_TYPE>;
	static const u8 kBinormalCount = BINORMAL_COUNT;

	static VertexLayout s_layout() {
		static VertexLayoutCreator<VertexT_Binormal> s;
		return s.layout;
	}

	static void onCreateLayout(VertexLayoutDesc* desc) {
		BASE::onCreateLayout(desc);
		desc->addElement(VertexSemantic::BINORMAL0, &VertexT_Binormal::binormal);
	}
};

template<class TANGENT_TYPE, u8 TANGENT_COUNT, class BASE>
struct VertexT_Tangent : public BASE
{
	using TangentType = TANGENT_TYPE;
	FixedArray<TANGENT_TYPE, TANGENT_COUNT>	tangent;

	static const RenderDataType	kTangentType = RenderDataType_get<TANGENT_TYPE>;
	static const u8 kTangentCount = TANGENT_COUNT;

	static VertexLayout s_layout() {
		static VertexLayoutCreator<VertexT_Tangent> s;
		return s.layout;
	}

	static void onCreateLayout(VertexLayoutDesc* desc) {
		BASE::onCreateLayout(desc);
		desc->addElement(VertexSemantic::TANGENT0, &VertexT_Tangent::tangent);
	}
};


//----- COUNT = 0 -----
template<class COLOR_TYPE   , class BASE> struct VertexT_Color<   COLOR_TYPE   , 0, BASE> : public BASE {};
template<class UV_TYPE      , class BASE> struct VertexT_Uv<      UV_TYPE      , 0, BASE> : public BASE {};
template<class NORMAL_TYPE  , class BASE> struct VertexT_Normal<  NORMAL_TYPE  , 0, BASE> : public BASE {};
template<class BINORMAL_TYPE, class BASE> struct VertexT_Binormal<BINORMAL_TYPE, 0, BASE> : public BASE {};
template<class TANGENT_TYPE , class BASE> struct VertexT_Tangent< TANGENT_TYPE , 0, BASE> : public BASE {};
//----------

template<class POS_TYPE,
	class COLOR_TYPE   , u8 COLOR_COUNT   ,
	class UV_TYPE      , u8 UV_COUNT      ,
	class NORMAL_TYPE  , u8 NORMAL_COUNT  ,
	class BINORMAL_TYPE, u8 BINORMAL_COUNT,
	class TANGENT_TYPE , u8 TANGENT_COUNT>
using Vertex_DetailMake = 
	VertexT_Tangent<TANGENT_TYPE, TANGENT_COUNT,
	VertexT_Binormal<BINORMAL_TYPE, BINORMAL_COUNT,
	VertexT_Normal<NORMAL_TYPE, NORMAL_COUNT,
	VertexT_Uv<UV_TYPE, UV_COUNT,
	VertexT_Color<COLOR_TYPE, COLOR_COUNT,
	VertexT_Pos<POS_TYPE> > > > > >;

template<
	u8 COLOR_COUNT   ,
	u8 UV_COUNT      ,
	u8 NORMAL_COUNT  ,
	u8 BINORMAL_COUNT,
	u8 TANGENT_COUNT>
using Vertex_MakeBase = Vertex_DetailMake<
	Vec3f,
	Color4b, COLOR_COUNT, 
	Vec2f  , UV_COUNT, 
	Vec3f  , NORMAL_COUNT,
	Vec3f  , BINORMAL_COUNT,
	Vec3f  , TANGENT_COUNT>;

template<u8 COLOR_COUNT, u8 UV_COUNT, VertexNormalCount NORMAL_COUNT>
struct Vertex_Make_Struct;

template<u8 COLOR_COUNT, u8 UV_COUNT>
struct Vertex_Make_Struct<COLOR_COUNT, UV_COUNT, VertexNormalCount::None> {
	using Type = Vertex_MakeBase<COLOR_COUNT, UV_COUNT, 0, 0, 0>;
};

template<u8 COLOR_COUNT, u8 UV_COUNT>
struct Vertex_Make_Struct<COLOR_COUNT, UV_COUNT, VertexNormalCount::Normal> {
	using Type = Vertex_MakeBase<COLOR_COUNT, UV_COUNT, 1, 0, 0>;
};

template<u8 COLOR_COUNT, u8 UV_COUNT>
struct Vertex_Make_Struct<COLOR_COUNT, UV_COUNT, VertexNormalCount::Binormal> {
	using Type = Vertex_MakeBase<COLOR_COUNT, UV_COUNT, 1, 1, 0>;
};

template<u8 COLOR_COUNT, u8 UV_COUNT>
struct Vertex_Make_Struct<COLOR_COUNT, UV_COUNT, VertexNormalCount::Tangent> {
	using Type = Vertex_MakeBase<COLOR_COUNT, UV_COUNT, 1, 1, 1>;
};

template<u8 COLOR_COUNT, u8 UV_COUNT, VertexNormalCount NORMAL_COUNT>
using Vertex_Simple = typename Vertex_Make_Struct<COLOR_COUNT, UV_COUNT, NORMAL_COUNT>::Type;


using Vertex_Pos2f	= VertexT_Pos<Vec2f>;
using Vertex_ImGui	= VertexT_Color<Color4b, 1, VertexT_Uv<Vec2f, 1, Vertex_Pos2f>>;

using Vertex_Pos					= Vertex_Simple<0, 0, VertexNormalCount::None>;
using Vertex_PosUv					= Vertex_Simple<0, 1, VertexNormalCount::None>;
using Vertex_PosUv2					= Vertex_Simple<0, 2, VertexNormalCount::None>;
using Vertex_PosNormal				= Vertex_Simple<0, 0, VertexNormalCount::Normal>;
using Vertex_PosUvNormal			= Vertex_Simple<0, 1, VertexNormalCount::Normal>;

using Vertex_PosColor				= Vertex_Simple<1, 0, VertexNormalCount::None>;
using Vertex_PosColorNormal			= Vertex_Simple<1, 0, VertexNormalCount::Normal>;
using Vertex_PosColorBinormal		= Vertex_Simple<1, 0, VertexNormalCount::Binormal>;

using Vertex_PosColorUv				= Vertex_Simple<1, 1, VertexNormalCount::None>;
using Vertex_PosColorUvNormal		= Vertex_Simple<1, 1, VertexNormalCount::Normal>;
using Vertex_PosColorUvBinormal		= Vertex_Simple<1, 1, VertexNormalCount::Binormal>;

using Vertex_PosColorUv2			= Vertex_Simple<1, 2, VertexNormalCount::None>;
using Vertex_PosColorUv2Normal		= Vertex_Simple<1, 2, VertexNormalCount::Normal>;
using Vertex_PosColorUv2Binormal	= Vertex_Simple<1, 2, VertexNormalCount::Binormal>;

}
