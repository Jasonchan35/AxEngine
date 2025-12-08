module;
export module AxRender:Renderer_Null;
export import :Renderer_Backend;

export namespace ax::AxRender {

class Renderer_Null : public Renderer_Backend {
	AX_RTTI_INFO(Renderer_Null, Renderer_Backend)
public:
	AX_DOWNCAST_GET_INSTANCE()

	Renderer_Null(const CreateDesc& desc) : Base(desc) {}
	virtual ~Renderer_Null() override { destroy(); }

	AX_Renderer_FunctionInterfaces_override(Null);
};


} // namespace

