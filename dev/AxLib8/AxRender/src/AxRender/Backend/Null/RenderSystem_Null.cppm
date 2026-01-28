module;
export module AxRender:RenderSystem_Null;
export import :RenderSystem_Backend;
export import :GpuBuffer_Backend;
export import :RenderContext_Backend;
export import :RenderObjectManager_Backend;
export import :RenderPass_Backend;
export import :RenderRequest_Backend;
export import :Texture_Backend;
export import :MeshObject_Backend;
export import :Material_Backend;
export import :ImageIO;

export namespace ax /*::AxRender*/ {

class RenderSystem_Null : public RenderSystem_Backend {
	AX_RTTI_INFO(RenderSystem_Null, RenderSystem_Backend)
public:
	AX_DOWNCAST_GET_INSTANCE()

	RenderSystem_Null(const CreateDesc& desc) : Base(desc) {}
	virtual ~RenderSystem_Null() override { destroy(); }

	AX_RenderSystem_FunctionInterfaces(Null, override)
};

class RenderContext_Null : public RenderContext_Backend {
	AX_RTTI_INFO(RenderContext_Null, RenderContext_Backend)
public:
	RenderContext_Null(const CreateDesc& desc) : Base(desc) {}
	virtual ~RenderContext_Null() override = default;
protected:
	virtual Vec2f	worldToLocalPos(const Vec2f& pt) override { return TagZero; }
	virtual Vec2f	localToWorldPos(const Vec2f& pt) override { return TagZero; }
	virtual RenderPass_Backend* onAcquireBackBufferRenderPass(RenderRequest* req) override { return nullptr; }
	virtual void	onPresentSurface(RenderRequest* req) override {}
	virtual void	onCreateSwapChain() override {}
};

class RenderObjectManager_Null : public RenderObjectManager_Backend {
	AX_RTTI_INFO(RenderObjectManager_Null, RenderObjectManager_Backend)
public:
	RenderObjectManager_Null(const CreateDesc& desc) : Base(desc) {}
};

class GpuBufferPool_Null : public GpuBufferPool_Backend {
	AX_RTTI_INFO(GpuBufferPool_Null, GpuBufferPool_Backend)
public:
	GpuBufferPool_Null(const CreateDesc& desc) : Base(desc) {}
};

class GpuBuffer_Null : public GpuBuffer_Backend {
	AX_RTTI_INFO(GpuBuffer_Null, GpuBuffer_Backend)
public:
	GpuBuffer_Null(const CreateDesc& desc) : Base(desc) {}

	virtual MutByteSpan	onMapMemory(IntRange range) override { return MutByteSpan(); }
	virtual void		onUnmapMemory() override {}
	virtual void		onFlush(IntRange range) override {}
	virtual void		onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) override {}
};

class GpuStructuredBuffer_Null : public GpuStructuredBuffer_Backend {
	AX_RTTI_INFO(GpuStructuredBuffer_Null, GpuStructuredBuffer_Backend)
public:
	GpuStructuredBuffer_Null(const CreateDesc& desc) : Base(desc) {}
};

class RenderPassColorBuffer_Null : public RenderPassColorBuffer_Backend {
	AX_RTTI_INFO(RenderPassColorBuffer_Null, RenderPassColorBuffer_Backend)
public:
	RenderPassColorBuffer_Null(const CreateDesc& desc) : Base(desc) {}
};

class RenderPassDepthBuffer_Null : public RenderPassDepthBuffer_Backend {
	AX_RTTI_INFO(RenderPassDepthBuffer_Null, RenderPassDepthBuffer_Backend)
public:
	RenderPassDepthBuffer_Null(const CreateDesc& desc) : Base(desc) {}
};

class RenderPass_Null : public RenderPass_Backend {
	AX_RTTI_INFO(RenderPass_Null, RenderPass_Backend)
public:
	RenderPass_Null(const CreateDesc& desc) : Base(desc) {}
};

class RenderRequest_Null : public RenderRequest_Backend {
	AX_RTTI_INFO(RenderRequest_Null, RenderRequest_Backend)
public:
	RenderRequest_Null(const CreateDesc& desc) : Base(desc) {}

	AX_RenderRequest_Backend_FunctionInterfaces(override {})
};

class Sampler_Null : public Sampler_Backend {
	AX_RTTI_INFO(Sampler_Null, Sampler_Backend)
public:
	Sampler_Null(const CreateDesc& desc) : Base(desc) {}
};

class Texture2D_Null : public Texture2D_Backend {
	AX_RTTI_INFO(Texture2D_Null, Texture2D_Backend)
public:
	Texture2D_Null(const CreateDesc& desc) : Base(desc) {}

	virtual void onImageIO_ReadHandler(class ImageIO_ReadHandler& handler) override {
		auto dataSize = handler.desc.dataSize;
		ByteArray buf;
		buf.resize(dataSize);
		handler.readPixelsTo(buf);
	}
};

class ShaderParamSpace_Null : public ShaderParamSpace_Backend {
	AX_RTTI_INFO(ShaderParamSpace_Null, ShaderParamSpace_Backend)
public:
	ShaderParamSpace_Null(const CreateDesc& desc) : Base(desc) {}
};

class MaterialParamSpace_Null : public MaterialParamSpace_Backend {
	AX_RTTI_INFO(MaterialParamSpace_Null, MaterialParamSpace_Backend)
public:
	MaterialParamSpace_Null(const CreateDesc& desc) : Base(desc) {}	
};

class ShaderPass_Null : public ShaderPass_Backend {
	AX_RTTI_INFO(ShaderPass_Null, ShaderPass_Backend)
public:
	ShaderPass_Null(const CreateDesc& desc) : Base(desc) {}	
};

class Shader_Null : public Shader_Backend {
	AX_RTTI_INFO(Shader_Null, Shader_Backend)
public:
	Shader_Null(const CreateDesc& desc) : Base(desc) {}
	virtual UPtr<ShaderPass_Backend > onNewPass (const ShaderPass_CreateDesc& desc) override {
		return UPtr_new<ShaderPass_Null>(AX_NEW, desc);
	}
};

class MaterialPass_Null : public MaterialPass_Backend {
	AX_RTTI_INFO(MaterialPass_Null, MaterialPass_Backend)
public:
	MaterialPass_Null(const CreateDesc& desc) : Base(desc) {}
	virtual bool onBindMaterial(class RenderRequest* req, AxDrawCallDesc& cmd) override { return false; }
};

class Material_Null : public Material_Backend {
	AX_RTTI_INFO(Material_Null, Material_Backend)
public:
	Material_Null(const CreateDesc& desc) : Base(desc) {}
	virtual UPtr<MaterialPass_Backend>	onNewPass(const MaterialPass_CreateDesc& desc) override {
		return UPtr_new<MaterialPass_Null>(AX_NEW, desc);
	}
};

class MeshObject_Null : public MeshObject_Backend {
	AX_RTTI_INFO(MeshObject_Null, MeshObject_Backend)
public:
	MeshObject_Null(const CreateDesc& desc) : Base(desc) {}
};

} // namespace

