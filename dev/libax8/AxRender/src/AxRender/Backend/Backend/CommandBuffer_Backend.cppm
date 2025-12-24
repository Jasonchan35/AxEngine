module;
export module AxRender:CommandBuffer_Backend;
export import :RenderSystem_Backend;
export import :CommandBuffer;

export namespace ax /*::AxRender*/ {

class CommandBuffer_Backend : public CommandBuffer {
	AX_RTTI_INFO(CommandBuffer_Backend, CommandBuffer)
};

} // namespace

