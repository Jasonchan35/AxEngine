module;
export module AxRender:StockObjects;
export import :Texture;

export namespace ax /*::AxRender*/ {

#define AX_RenderStockTextureId_ENUM_LIST(E) \
	E(None,) \
	E(Error,) \
	AX_RenderStockTextureId_SolidColor_ENUM_LIST(E) \
//----

AX_ENUM_CLASS(AX_RenderStockTextureId_ENUM_LIST, RenderStockTextureId, u8)

class RenderStockObjects : public NonCopyable {
public:
	struct Texture2Ds : public NonCopyable {
		Texture2Ds();

	#define E(T,...) SPtr<Texture2D> k##T;
		AX_RenderStockTextureId_ENUM_LIST(E)
	#undef E


		Texture2D*	get(RenderStockTextureId texId);
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

	static RenderStockObjects* s_instance();

	static void s_create();
	static void s_destroy();

protected:
	RenderStockObjects();
};

} // namespace


