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
struct VertexT_Normal : public BASE
{
	using NormalType = NORMAL_TYPE;
	FixedArray<NORMAL_TYPE, NORMAL_COUNT>	normal;

	static const RenderDataType	kNormalType    = DataType_get<NORMAL_TYPE>();
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

template<class TANGENT_TYPE, u8 TANGENT_COUNT, class BASE>
struct VertexT_Tangent : public BASE
{
	using TangentType = TANGENT_TYPE;
	FixedArray<TANGENT_TYPE, TANGENT_COUNT>	tangent;

	static const RenderDataType	kTangentType    = DataType_get<TANGENT_TYPE>();
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

template<class BINORMAL_TYPE, u8 BINORMAL_COUNT, class BASE>
struct VertexT_Binormal : public BASE
{
	using BinormalType = BINORMAL_TYPE;
	FixedArray<BINORMAL_TYPE, BINORMAL_COUNT>	binormal;

	static const RenderDataType	kBinormalType    = DataType_get<BINORMAL_TYPE>();
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

using Vertex_Pos2f				= VertexT_Pos<Vec2f_Basic>;

using Vertex_Pos				= VertexT_Pos<Vec3f_Basic>;
using Vertex_PosNormal			= VertexT_Normal<Vec3f_Basic, 1, Vertex_Pos>;

template<u8 COLOR>
using Vertex_PosColor_			= VertexT_Color<Color4b_Basic,  COLOR, Vertex_Pos>;
using Vertex_PosColor			= Vertex_PosColor_<1>;

template<u8 COLOR>
using Vertex_PosNormalColor_	= VertexT_Normal<Color3b_Basic, COLOR, Vertex_PosColor>;
using Vertex_PosNormalColor		= Vertex_PosNormalColor_<1>;

template<u8 UV>
using Vertex_PosUv_	= VertexT_Uv<Vec2f_Basic, UV, Vertex_Pos>;
using Vertex_PosUv	= Vertex_PosUv_<1>;

template<u8 UV, u8 COLOR>
using Vertex_PosUvColor_		= VertexT_Color<Color4b_Basic, COLOR, Vertex_PosUv_<UV>>;
using Vertex_PosUvColor			= Vertex_PosUvColor_<1, 1>;

template<u8 UV>				
using Vertex_PosNormalUv_		= VertexT_Normal<Vec3f_Basic, 1, Vertex_PosUv_<UV>>;
using Vertex_PosNormalUv		= Vertex_PosNormalUv_<1>;

template<u8 UV, u8 COLOR>	
using Vertex_PosNormalUvColor_	= VertexT_Normal<Vec3f_Basic, 1, Vertex_PosUvColor_<UV, COLOR>>;
using Vertex_PosNormalUvColor	= Vertex_PosNormalUvColor_<1, 1>;

template<u8 UV>				
using Vertex_PosTangentUv_		= VertexT_Tangent<Vec3f_Basic, 1, Vertex_PosNormalUv_<UV>>;
using Vertex_PosTangentUv		= Vertex_PosTangentUv_<1>;

template<u8 UV, u8 COLOR>	
using Vertex_PosTangentUvColor_	= VertexT_Tangent<Vec3f_Basic, 1, Vertex_PosNormalUvColor_<UV, COLOR>>;
using Vertex_PosTangentUvColor	= Vertex_PosTangentUvColor_<1, 1>;

using Vertex_ImGui = VertexT_Color<Color4b_Basic, 1, VertexT_Uv<Vec2f_Basic, 1, Vertex_Pos2f>>;

}
