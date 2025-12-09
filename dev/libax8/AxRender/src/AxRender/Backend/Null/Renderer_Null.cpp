module;

module AxRender;
import :Renderer_Null;

import :GpuBuffer_Backend;
import :RenderContext_Backend;
import :RenderPass_Backend;
import :RenderRequest_Backend;
import :Texture_Backend;
import :Material_Backend;

namespace ax::AxRender {

class GpuBuffer_Null : public GpuBuffer_Backend {
	AX_RTTI_INFO(GpuBuffer_Null, GpuBuffer_Backend)
public:
	GpuBuffer_Null(const CreateDesc& desc) : Base(desc) {}

	virtual MutByteSpan	onMapMemory(IntRange range) { return MutByteSpan(); }
	virtual void		onUnmapMemory() {}
	virtual void		onFlush(IntRange range) {}
	virtual void		onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) {}
};

class RenderColorBuffer_Null : public RenderColorBuffer_Backend {
	AX_RTTI_INFO(RenderColorBuffer_Null, RenderColorBuffer_Backend)
public:
	RenderColorBuffer_Null(const CreateDesc& desc) : Base(desc) {}
};

class RenderDepthBuffer_Null : public RenderDepthBuffer_Backend {
	AX_RTTI_INFO(RenderDepthBuffer_Null, RenderDepthBuffer_Backend)
public:
	RenderDepthBuffer_Null(const CreateDesc& desc) : Base(desc) {}
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

	virtual void onImageIO_ReadResult(class ImageIO_ReadResult& result) {}
};

class Texture3D_Null : public Texture3D_Backend {
	AX_RTTI_INFO(Texture3D_Null, Texture3D_Backend)
public:
	Texture3D_Null(const CreateDesc& desc) : Base(desc) {}
};

class TextureCube_Null : public TextureCube_Backend {
	AX_RTTI_INFO(TextureCube_Null, TextureCube_Backend)
public:
	TextureCube_Null(const CreateDesc& desc) : Base(desc) {}
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
	virtual UPtr<ShaderPass_Backend > onNewPass (const ShaderPass_Backend_CreateDesc& desc) {
		return UPtr_new<ShaderPass_Null>(AX_ALLOC_REQ, desc);
	}
};

class MaterialPass_Null : public MaterialPass_Backend {
	AX_RTTI_INFO(MaterialPass_Null, MaterialPass_Backend)
public:
	MaterialPass_Null(const CreateDesc& desc) : Base(desc) {}
	virtual bool onDrawcall(class RenderRequest* req, Cmd_DrawCall& cmd) { return false; }
};

class Material_Null : public Material_Backend {
	AX_RTTI_INFO(Material_Null, Material_Backend)
public:
	Material_Null(const CreateDesc& desc) : Base(desc) {}
	virtual UPtr<MaterialPass_Backend>	onNewPass(const MaterialPass_Backend_CreateDesc& desc) {
		return UPtr_new<MaterialPass_Null>(AX_ALLOC_REQ, desc);
	}
};

class RenderContext_Null : public RenderContext_Backend {
	AX_RTTI_INFO(RenderContext_Null, RenderContext_Backend)
public:
	RenderContext_Null(const CreateDesc& desc) : Base(desc) {}
	virtual ~RenderContext_Null() override = default;
protected:
	virtual Vec2f	worldToLocalPos(const Vec2f& pt) override { return AX_ZERO; }
	virtual Vec2f	localToWorldPos(const Vec2f& pt) override { return AX_ZERO; }
	virtual BackBuffer*	onGetBackBuffer(Int i) override { return nullptr; }
	virtual RenderPass_Backend* onAcquireBackBufferRenderPass(RenderRequest* req) override { return nullptr; }
	virtual void onPresentSurface(RenderRequest* req) override {}
};

AX_Renderer_FunctionBodies(Null)

} // namespace