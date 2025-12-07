module;
export module AxRender.RenderPass_Backend;
export import AxRender.RenderPass;

export namespace ax::AxRender {

class RenderPass_Backend : public RenderPass {
	AX_RTTI_INFO(RenderPass_Backend, RenderPass)
public:	
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

protected:
	RenderPass_Backend(const CreateDesc& desc) : Base(desc) {}
};

} // namespace
