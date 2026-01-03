module;
export module AxRender:Material;
export import :Shader;

export namespace ax /*::AxRender*/ {

class Material_CreateDesc {};

class Material : public RenderObject {
	AX_RTTI_INFO(Material, RenderObject)
public:
	using BindSpace = ShaderParamBindSpace;
	using CreateDesc = Material_CreateDesc;

	static SPtr<This> s_new(const MemAllocRequest& req, Shader* shader = nullptr);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView shaderAssetPath);
	
	void setShader(Shader* shader);
	Shader* shader();

	bool setParam(NameId name, const i32&		v);
	bool setParam(NameId name, const Vec1i32_Basic&	v);	
	bool setParam(NameId name, const Vec2i32_Basic&	v);
	bool setParam(NameId name, const Vec3i32_Basic&	v);
	bool setParam(NameId name, const Vec4i32_Basic&	v);

	bool setParam(NameId name, const u32&			v);
	bool setParam(NameId name, const Vec1u32_Basic&	v);
	bool setParam(NameId name, const Vec2u32_Basic&	v);
	bool setParam(NameId name, const Vec3u32_Basic&	v);
	bool setParam(NameId name, const Vec4u32_Basic&	v);

	bool setParam(NameId name, const f32&			v);
	bool setParam(NameId name, const Vec1f_Basic& 	v);	
	bool setParam(NameId name, const Vec2f_Basic& 	v);
	bool setParam(NameId name, const Vec3f_Basic& 	v);
	bool setParam(NameId name, const Vec4f_Basic& 	v);

	bool setParam(NameId name, const f64&			v);
	bool setParam(NameId name, const Vec1d_Basic&	v);	
	bool setParam(NameId name, const Vec2d_Basic&	v);
	bool setParam(NameId name, const Vec3d_Basic&	v);
	bool setParam(NameId name, const Vec4d_Basic&	v);

	bool setParam(NameId name, const Mat4f_Basic&	v);
	bool setParam(NameId name, const Mat4d_Basic&	v);

	bool setParam(NameId name, const Color3f_Basic&	v);
	bool setParam(NameId name, const Color4f_Basic&	v);

	bool setParam(NameId name, Sampler*			v);
	bool setParam(NameId name, Texture2D*		v);

protected:
	Material(const CreateDesc& desc);
};

} // namespace