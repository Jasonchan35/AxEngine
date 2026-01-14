module AxRender;
import :RenderMeshEdit;

namespace ax /*::AxRender*/ {

static RenderStockObjects* StockObjects_instance = nullptr;

RenderStockObjects::RenderStockObjects() {
}

RenderStockObjects::Fallback::Fallback() {
	{
		Sampler_CreateDesc desc;
		desc.isFallbackDefault = true;
		sampler = Sampler::s_new(AX_NEW, desc);
	}
	{
		Image image;
		image.create(ColorType::RGBAb, Vec2i(4,4));
		image.fill(Color4b::kMagenta());
		texture2D = Texture2D::s_new(AX_NEW, image);
		texture2D->setDebugName("fallback.Texture2D");
	}
}

RenderStockObjects::Samplers::Samplers() 
: Point		  (SamplerFilter:: Point		 )
, Linear	  (SamplerFilter:: Linear	 	 )
, Bilinear	  (SamplerFilter:: Bilinear	 	 )
, Trilinear	  (SamplerFilter:: Trilinear	 )
, Anisotropic (SamplerFilter:: Anisotropic	 )

{
	Sampler_CreateDesc desc;
	defaultValue = Sampler::s_new(AX_NEW, desc);
}

RenderStockObjects::Samplers::FilterSet::FilterSet(SamplerFilter filter) {
	Clamp      = Sampler::s_new(AX_NEW, filter, SamplerWrapUVW(TagAll, SamplerWrap::Clamp     ));
	Repeat     = Sampler::s_new(AX_NEW, filter, SamplerWrapUVW(TagAll, SamplerWrap::Repeat    ));
	Mirror     = Sampler::s_new(AX_NEW, filter, SamplerWrapUVW(TagAll, SamplerWrap::Mirror    ));
	MirrorOnce = Sampler::s_new(AX_NEW, filter, SamplerWrapUVW(TagAll, SamplerWrap::MirrorOnce));
}

RenderStockObjects::Meshes::Meshes() {
	axis = MeshObject::s_new(AX_NEW);
	RenderMeshEdit(axis->meshData).createAxis(Vertex_PosColor::s_layout());

	//----
	auto layout = Vertex_PosNormalUv::s_layout();

	cube = MeshObject::s_new(AX_NEW);
	RenderMeshEdit(cube->meshData).createCube(layout, Vec3f::s_zero(), Vec3f::s_one(), Color4f::kWhite());
	
	sphere = MeshObject::s_new(AX_NEW);
	RenderMeshEdit(sphere->meshData).createSphere(layout, 1, 32, 32);
	
	cone = MeshObject::s_new(AX_NEW);
	RenderMeshEdit(cone->meshData).createCone(layout, 1, 1, 2, 32, true);
	
	cylinder = MeshObject::s_new(AX_NEW);
	RenderMeshEdit(cylinder->meshData).createCylinder(layout, 1, 1, 2, 32, true, true);
}

RenderStockObjects* RenderStockObjects::s_instance() { return StockObjects_instance; }

void RenderStockObjects::s_create() {
	AX_ASSERT(StockObjects_instance == nullptr);
	StockObjects_instance = new(AX_NEW) RenderStockObjects();
}

void RenderStockObjects::s_destroy() {
//	AX_ASSERT(StockObjects_instance);
	if (StockObjects_instance) {
		AxDelete::deleteObject(StockObjects_instance);
		StockObjects_instance = nullptr;
	}
}

RenderStockObjects::Font::Font() {
	defaultFontStyle = SPtr_new<FontStyle>(AX_NEW);
}

RenderStockObjects::Texture2Ds::Texture2Ds() {
	{
		Image image;
		auto createTex = [&](SPtr<Texture2D> & outTex, const Color4b& color, StrView name) {
			image.create(ColorType::RGBAb, Vec2i(4,4));
			image.fill(color);
			outTex = Texture2D::s_new(AX_NEW, image);
			outTex->setDebugName(name);
		};

		#define E(T,...) createTex(k##T, Color4b::k##T(), "SolidColor-"#T);
			AX_RenderStockTextureId_SolidColor_ENUM_LIST(E)
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
		kError = Texture2D::s_new(AX_NEW, image);
	}
}

Texture2D* RenderStockObjects::Texture2Ds::get(RenderStockTextureId texId) {
	switch (texId) {
	#define E(T, ...) case RenderStockTextureId::T: return k##T;
		AX_RenderStockTextureId_SolidColor_ENUM_LIST(E)
	#undef E

		default: AX_ASSERT(false); return nullptr;
	}
}

} // namespace
