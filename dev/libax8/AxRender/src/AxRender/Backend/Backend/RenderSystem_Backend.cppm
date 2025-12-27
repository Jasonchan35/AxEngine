module;
#include <imgui.h>
export module AxRender:RenderSystem_Backend;
export import :Renderer;

export namespace ax /*::AxRender*/ {

class RenderRequest_Backend;

template<class T> class BindlessTable;

class RenderSystem_Backend : public RenderSystem {
	AX_RTTI_INFO(RenderSystem_Backend, RenderSystem)
public:
	AX_DOWNCAST_GET_INSTANCE()

	RenderSystem_Backend(const CreateDesc& desc);
	virtual ~RenderSystem_Backend() override;

	AX_RenderSystem_FunctionInterfaces(AX_EMPTY, =0)

	RenderRequest_Backend*		nextRenderRequest();

	ImFontAtlas* imguiFontAtlas() { return &_imguiFontAtlas; }

	void waitAllRenderCompleted();

	RenderRequest_Backend* getRenderRequest(Int i);

protected:
	virtual void onCreate() override;
	virtual void onDestroy() override;
	virtual void onFileChanged(FileDirWatcher_Result& result) override;

	ImFontAtlas _imguiFontAtlas;
private:
	struct PrivateData;
	UPtr<PrivateData>	_privateData;
};


} // namespace
