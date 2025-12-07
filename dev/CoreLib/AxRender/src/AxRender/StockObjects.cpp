module AxRender.StockObjects;

namespace ax::AxRender {

static StockObjects* StockObjects_instance = nullptr;

StockObjects::StockObjects() {
}

StockObjects::Samplers::Samplers() 
: Point		  (SamplerFilter:: Point		 )
, Linear	  (SamplerFilter:: Linear	 	 )
, Bilinear	  (SamplerFilter:: Bilinear	 	 )
, Trilinear	  (SamplerFilter:: Trilinear	 )
, Anisotropic (SamplerFilter:: Anisotropic	 )

{
	Sampler_CreateDesc desc;
	defaultValue = Sampler::s_new(AX_ALLOC_REQ, desc);
}

StockObjects::Samplers::FilterSet::FilterSet(SamplerFilter filter) {
	Clamp      = Sampler::s_new(AX_ALLOC_REQ, filter, SamplerWrap::Clamp     );
	Repeat     = Sampler::s_new(AX_ALLOC_REQ, filter, SamplerWrap::Repeat    );
	Mirror     = Sampler::s_new(AX_ALLOC_REQ, filter, SamplerWrap::Mirror    );
	MirrorOnce = Sampler::s_new(AX_ALLOC_REQ, filter, SamplerWrap::MirrorOnce);
}

StockObjects* StockObjects::s_instance() { return StockObjects_instance; }

void StockObjects::s_create() {
	AX_ASSERT(StockObjects_instance == nullptr);
	StockObjects_instance = new(AX_ALLOC_REQ) StockObjects();
}

void StockObjects::s_destroy() {
	AX_ASSERT(StockObjects_instance);
	AxDelete::deleteObject(StockObjects_instance);
	StockObjects_instance = nullptr;
}

StockObjects::Texture2Ds::Texture2Ds() {
	{
		Image image;
		auto createTex = [&](SPtr<Texture2D> & outTex, const Color4b& color) {
			image.create(ColorType::RGBAb, Vec2i(4,4));
			image.fill(color);
			outTex = Texture2D::s_new(AX_ALLOC_REQ, image);
		};

		#define E(T,...) createTex(k##T, Color4b::k##T());
			AX_RENDER_StockTextureId_SolidColor_ENUM_LIST(E)
		#undef E
	}

	{
		Image image;
		image.create(ColorType::RGBAb, Vec2i(4,4));
		for (Int y = 0; y < 4; y++) {
			auto row = image.row<ColorRGBAb>(y);
			for (Int x = 0; x < 4; x++) {
				auto v = static_cast<Byte>((x + y) % 2 * 255);
				row[x] = ColorRGBAb(v, 0, v, 255);
			}
		}
		kError = Texture2D::s_new(AX_ALLOC_REQ, image);
	}
}

Texture2D* StockObjects::Texture2Ds::get(StockTextureId texId) {
	switch (texId) {
	#define E(T, ...) case StockTextureId::T: return k##T;
		AX_RENDER_StockTextureId_SolidColor_ENUM_LIST(E)
	#undef E

		default: AX_ASSERT(false); return nullptr;
	}
}

} // namespace
