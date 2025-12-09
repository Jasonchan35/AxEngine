module;
export module AxRender:Material;
export import :Shader;

export namespace ax::AxRender {

class MaterialParamSpace_CreateDesc : public NonCopyable {
public:
	const ShaderParamSpace*	paramSpace = nullptr;
};

class MaterialParamSpace : public RenderObject {
	AX_RTTI_INFO(MaterialParamSpace, RenderObject)
public:
	using CreateDesc = MaterialParamSpace_CreateDesc;
protected:
	MaterialParamSpace(const CreateDesc& desc) {}

private:
};

class Material_CreateDesc {};

class Material : public RenderObject {
	AX_RTTI_INFO(Material, RenderObject)
public:
	using CreateDesc = Material_CreateDesc;

	static SPtr<This> s_new(const MemAllocRequest& req, Shader* shader = nullptr);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView shaderAssetPath);
	
	void setShader(Shader* shader);
	Shader* shader();


	bool setParam(NameId name, const i32&		v);
	bool setParam(NameId name, const i32x2&		v);
	bool setParam(NameId name, const i32x3&		v);
	bool setParam(NameId name, const i32x4&		v);

	bool setParam(NameId name, const u32&		v);
	bool setParam(NameId name, const u32x2&		v);
	bool setParam(NameId name, const u32x3&		v);
	bool setParam(NameId name, const u32x4&		v);

	bool setParam(NameId name, const f32&		v);
	bool setParam(NameId name, const f32x2&		v);
	bool setParam(NameId name, const f32x3&		v);
	bool setParam(NameId name, const f32x4&		v);

	bool setParam(NameId name, const f64&		v);
	bool setParam(NameId name, const f64x2&		v);
	bool setParam(NameId name, const f64x3&		v);
	bool setParam(NameId name, const f64x4&		v);

	bool setParam(NameId name, const Mat4f&		v);
	bool setParam(NameId name, const Mat4d&		v);

	bool setParam(NameId name, const Color3f&	v);
	bool setParam(NameId name, const Color4f&	v);

	bool setParam(NameId name, Sampler*			v);
	bool setParam(NameId name, Texture2D*		v);

protected:
	Material(const CreateDesc& desc);
};

} // namespace