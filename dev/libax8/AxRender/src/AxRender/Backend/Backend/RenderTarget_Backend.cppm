module;
export module AxRender:RenderTarget_Backend;
export import :Renderer_Backend;
export import :RenderTarget;

export namespace ax /*::AxRender*/ {

class RenderTargetColorBuffer_Backend : public RenderTargetColorBuffer {
	AX_RTTI_INFO(RenderTargetColorBuffer_Backend, RenderTargetColorBuffer)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

protected:
	RenderTargetColorBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
};

class RenderTargetDepthBuffer_Backend : public RenderTargetDepthBuffer {
	AX_RTTI_INFO(RenderTargetDepthBuffer_Backend, RenderTargetDepthBuffer)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

protected:
	RenderTargetDepthBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
};


} // namespace
