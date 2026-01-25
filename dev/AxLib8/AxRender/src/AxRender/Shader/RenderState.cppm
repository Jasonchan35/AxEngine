module;
export module AxRender:RenderState;
export import :RenderDataType;

export namespace ax /*::AxRender*/ {

#define AX_RenderState_DepthTestOp_ENUM_LIST(E) \
	E(Less,) \
	E(Equal,) \
	E(LessEqual,) \
	E(Greater,) \
	E(GreaterEqual,) \
	E(NotEqual,) \
	E(Always,) \
	E(Never,) \
	E(_END,) \
//-----
AX_ENUM_CLASS(AX_RenderState_DepthTestOp_ENUM_LIST, RenderState_DepthTestOp, u8)

#define AX_RenderState_BlendOp_ENUM_LIST(E) \
	E(Disable,) \
	E(Add,) \
	E(Sub,) \
	E(RevSub,) \
	E(Min,) \
	E(Max,) \
	E(_END,) \
//-----
AX_ENUM_CLASS(AX_RenderState_BlendOp_ENUM_LIST, RenderState_BlendOp, u8)

#define AX_RenderState_BlendFactor_ENUM_LIST(E) \
	E(Zero,) \
	E(One,) \
	E(SrcColor,) \
	E(DstColor,) \
	E(SrcAlpha,) \
	E(DstAlpha,) \
	E(ConstColor,) \
/*	E(ConstAlpha,) */ \
	E(OneMinusSrcColor,) \
	E(OneMinusDstColor,) \
	E(OneMinusSrcAlpha,) \
	E(OneMinusDstAlpha,) \
	E(OneMinusConstColor,) \
/*	E(OneMinusConstAlpha,) */\
	E(SrcAlphaSaturate,) \
	E(_END,) \
//-----
AX_ENUM_CLASS(AX_RenderState_BlendFactor_ENUM_LIST, RenderState_BlendFactor, u8)

#define AX_RenderState_CullMode_ENUM_LIST(E) \
	E(Back,) \
	E(Front,) \
	E(None,) \
//----
AX_ENUM_CLASS(AX_RenderState_CullMode_ENUM_LIST, RenderState_CullMode, u8)

class RenderState {
	using This = RenderState;
public:
	using Rect2       = Rect2f;
	using CullMode    = RenderState_CullMode;
	using DepthTestOp = RenderState_DepthTestOp;
	using BlendOp     = RenderState_BlendOp;
	using BlendFactor = RenderState_BlendFactor;

	static const Int kMaxRenderTargetCount = 8;

	CullMode	cull = CullMode::Back;
	bool		antialiasedLine = false;
	bool		wireframe = false;
	float		pointSize = 5;
	
	struct DepthTest {
		DepthTestOp op = ProjectionDesc().isReverseZ ? DepthTestOp::Greater : DepthTestOp::Less;
		bool writeMask = true;

		AX_INLINE
		bool operator==(const DepthTest& r) const {
			return op == r.op && writeMask == r.writeMask;
		}

		bool isEnable() const { return op != DepthTestOp::Always; }

		template<class SE>
		void onJsonIO(SE& se) {
			AX_JSON_IO(se, op);
			AX_JSON_IO(se, writeMask);
		}
	};
	DepthTest	depthTest;

	struct BlendFunc {
		BlendOp		op;
		BlendFactor	srcFactor;
		BlendFactor	dstFactor;

		void set(BlendOp op_, BlendFactor srcFactor_, BlendFactor dstFactor_) {
			op = op_;
			srcFactor = srcFactor_;
			dstFactor = dstFactor_;
		}

		template<class SE>
		void onJsonIO(SE& se) {
			AX_JSON_IO(se, op);
			AX_JSON_IO(se, srcFactor);
			AX_JSON_IO(se, dstFactor);
		}
	};

	struct Blend {
		BlendFunc	rgb;
		BlendFunc	alpha;
		Color4f		constColor;

		Blend();

		bool isEnable() const { return rgb.op != BlendOp::Disable || alpha.op != BlendOp::Disable; }

		template<class SE>
		void onJsonIO(SE& se) {
			AX_JSON_IO(se, rgb);
			AX_JSON_IO(se, alpha);
			AX_JSON_IO(se, constColor);
		}
	};
	Blend blend;

	template<class SE>
	void onJsonIO(SE& se) {
		AX_JSON_IO(se, cull);
		AX_JSON_IO(se, blend);
		AX_JSON_IO(se, depthTest);
		AX_JSON_IO(se, antialiasedLine);
		AX_JSON_IO(se, wireframe);
	}
};

} // namespace
