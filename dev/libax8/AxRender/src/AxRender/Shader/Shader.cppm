module;
export module AxRender:Shader;
export import :ShaderInfo;
export import :GpuBuffer;

export namespace ax::AxRender {

class Shader_CreateDesc : public NonCopyable {
public:
	StrView		assetPath;
};

class Shader : public RenderObject {
	AX_RTTI_INFO(Shader, RenderObject)
public:
	using CreateDesc = Shader_CreateDesc;
	using ResourceKey = String;

	const ResourceKey& resourceKey() const { return _assetPath; }

	Shader(const CreateDesc& desc);

	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView assetPath);

	const String& assetPath() const { return _assetPath; }
protected:

	String		_assetPath;
};

} // namespace 