module;
export module AxRender:RenderStockObjects;
export import :Texture;
export import :Font;
export import :RenderMesh;

export namespace ax /*::AxRender*/ {

#define AX_RenderStockTextureId_ENUM_LIST(E) \
	E(None,) \
	E(Error,) \
	AX_RenderStockTextureId_SolidColor_ENUM_LIST(E) \
//----
AX_ENUM_CLASS(AX_RenderStockTextureId_ENUM_LIST, RenderStockTextureId, u8)

class RenderStockObjects : public NonCopyable {
public:
	struct Fallback {
		Fallback();
		SPtr<Sampler>	sampler;
		SPtr<Texture2D> texture2D;
	}; 
	UPtr<Fallback> fallback;
	
	struct Font : public NonCopyable {
		Font();
		SPtr<FontStyle>		defaultFontStyle;
	};
	UPtr<Font> fonts;
	

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
	UPtr<Samplers>	samplers;

	struct Texture2Ds : public NonCopyable {
		Texture2Ds();

	#define E(T,...) SPtr<Texture2D> k##T;
		AX_RenderStockTextureId_ENUM_LIST(E)
	#undef E
		Texture2D*	get(RenderStockTextureId texId);
	};
	UPtr<Texture2Ds>	texture2Ds;

	struct Materials : public NonCopyable {
		Materials();
		SPtr<Material>	Simple3D_Unlit_Color;
		SPtr<Material>	Simple3D_Unlit_Texture;
		SPtr<Material>	Simple3D_Blinn_Color;
		SPtr<Material>	Simple3D_Blinn_Texture;
		SPtr<Material>	meshlet;
	};
	UPtr<Materials>	materials;
	
	
	struct Meshes : public NonCopyable {
		Meshes();
		SPtr<MeshObject>	Axis;
		SPtr<MeshObject>	Cube;
		SPtr<MeshObject>	Sphere;
		SPtr<MeshObject>	Cone;
		SPtr<MeshObject>	Cylinder;
	}; 
	UPtr<Meshes> meshes;
	
	static RenderStockObjects* s_instance();

	static void s_create();
	static void s_destroy();

protected:
	RenderStockObjects();
};

} // namespace


