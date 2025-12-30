module;
export module AxRender:RenderPass_Backend;
export import :RenderPass;

export namespace ax /*::AxRender*/ {

class RenderPassColorBuffer_Backend : public RenderPassColorBuffer {
	AX_RTTI_INFO(RenderPassColorBuffer_Backend, RenderPassColorBuffer)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

protected:
	RenderPassColorBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
};

class RenderPassDepthBuffer_Backend : public RenderPassDepthBuffer {
	AX_RTTI_INFO(RenderPassDepthBuffer_Backend, RenderPassDepthBuffer)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

protected:
	RenderPassDepthBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
};

class RenderPass_Backend : public RenderPass {
	AX_RTTI_INFO(RenderPass_Backend, RenderPass)
public:	
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

protected:
	RenderPass_Backend(const CreateDesc& desc) : Base(desc) {}
};

} // namespace
