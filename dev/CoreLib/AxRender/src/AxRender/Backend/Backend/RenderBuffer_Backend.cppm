module;
export module AxRender:RenderBuffer_Backend;
export import :RenderBuffer;

export namespace ax::AxRender {

class RenderColorBuffer_Backend : public RenderColorBuffer {
	AX_RTTI_INFO(RenderColorBuffer_Backend, RenderColorBuffer)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

protected:
	RenderColorBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
};

class RenderDepthBuffer_Backend : public RenderDepthBuffer {
	AX_RTTI_INFO(RenderDepthBuffer_Backend, RenderDepthBuffer)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

protected:
	RenderDepthBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
};

} // namespace