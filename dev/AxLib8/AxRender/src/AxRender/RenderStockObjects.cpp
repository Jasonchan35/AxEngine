module AxRender;
import :RenderMeshEdit;
import :Material_Backend;

namespace ax /*::AxRender*/ {

static RenderStockObjects* StockObjects_instance = nullptr;

RenderStockObjects::RenderStockObjects() {
	AX_ASSERT(StockObjects_instance == nullptr);
	StockObjects_instance = this;
	
	fallback.newObject(AX_NEW);
	fonts.newObject(AX_NEW);
	samplers.newObject(AX_NEW);
	texture2Ds.newObject(AX_NEW);

	materials.newObject(AX_NEW);
	meshes.newObject(AX_NEW);
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
		texture2D->setName("fallback.Texture2D");
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
	Axis = MeshObject::s_new(AX_NEW);
	Axis->setName("StockObj-Axis");
	RenderMeshEdit(Axis->renderMesh).createAxis(Vertex_PosColor::s_layout());

	//----
	auto layout = Vertex_PosColorUvNormal::s_layout();

	Cube = MeshObject::s_new(AX_NEW);
	Cube->setName("StockObj-Cube");
	RenderMeshEdit(Cube->renderMesh).createCube(layout, Vec3f::s_zero(), Vec3f::s_one(), Color4f::kWhite());
	
	Sphere = MeshObject::s_new(AX_NEW);
	Sphere->setName("StockObj-Sphere");
	RenderMeshEdit(Sphere->renderMesh).createSphere(layout, 1, 32, 32);
	
	Cone = MeshObject::s_new(AX_NEW);
	Cone->setName("StockObj-Cone");
	RenderMeshEdit(Cone->renderMesh).createCone(layout, 1, 1, 2, 32, true);
	
	Cylinder = MeshObject::s_new(AX_NEW);
	Cylinder->setName("StockObj-Cylinder");
	RenderMeshEdit(Cylinder->renderMesh).createCylinder(layout, 1, 1, 2, 32, true, true);
}

RenderStockObjects::Materials::Materials() {
	Simple3D_Unlit_Color   = Material::s_new(AX_NEW, "ImportedAssets/Shaders/core/Simple3D_Unlit_Color.axShader");
	Simple3D_Unlit_Texture = Material::s_new(AX_NEW, "ImportedAssets/Shaders/core/Simple3D_Unlit_Texture.axShader");
	Simple3D_Blinn_Color   = Material::s_new(AX_NEW, "ImportedAssets/Shaders/core/Simple3D_Blinn_Color.axShader");
	Simple3D_Blinn_Texture = Material::s_new(AX_NEW, "ImportedAssets/Shaders/core/Simple3D_Blinn_Texture.axShader");
	meshlet                = Material::s_new(AX_NEW, "ImportedAssets/Shaders/core/meshlet.axShader");
	meshlet_normal         = Material::s_new(AX_NEW, "ImportedAssets/Shaders/core/meshlet_normal.axShader");
}

RenderStockObjects* RenderStockObjects::s_instance() { return StockObjects_instance; }

void RenderStockObjects::s_create() {
	new(AX_NEW) RenderStockObjects();
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
			outTex->setName(name);
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
