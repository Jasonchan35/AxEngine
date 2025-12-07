module;
export module AxRender.StockObjects;
export import AxRender.Texture;

export namespace ax::AxRender {

#define AX_RENDER_StockTextureId_ENUM_LIST(E) \
	E(None,) \
	E(Error,) \
	AX_RENDER_StockTextureId_SolidColor_ENUM_LIST(E) \
//----

AX_ENUM_CLASS(AX_RENDER_StockTextureId_ENUM_LIST, StockTextureId, u8)

class StockObjects : public NonCopyable {
public:
	struct Texture2Ds : public NonCopyable {
		Texture2Ds();

	#define E(T,...) SPtr<Texture2D> k##T;
		AX_RENDER_StockTextureId_ENUM_LIST(E)
	#undef E


		Texture2D*	get(StockTextureId texId);
	};

	Texture2Ds	texture2Ds;

	struct Samplers : public NonCopyable {
		Samplers();
		SPtr<Sampler>	defaultValue;

		struct FilterSet {
			FilterSet(SamplerFilter filter);

			SPtr<Sampler>	Clamp;
			SPtr<Sampler>	Repeat;
			SPtr<Sampler>	Mirror;
			SPtr<Sampler>	MirrorOnce;
		};

		FilterSet	Point;
		FilterSet	Linear;
		FilterSet	Bilinear;
		FilterSet	Trilinear;
		FilterSet	Anisotropic;
	};
	Samplers	samplers;

	static StockObjects* s_instance();

	static void s_create();
	static void s_destroy();

protected:
	StockObjects();
};

} // namespace


