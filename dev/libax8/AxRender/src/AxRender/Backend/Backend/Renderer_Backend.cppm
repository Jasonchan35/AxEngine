module;
#include <imgui.h>
export module AxRender:Renderer_Backend;
export import :Renderer;

export namespace ax /*::AxRender*/ {

class RenderRequest_Backend;

template<class T> class BindlessTable;

class Renderer_Backend : public Renderer {
	AX_RTTI_INFO(Renderer_Backend, Renderer)
public:
	AX_DOWNCAST_GET_INSTANCE()

	Renderer_Backend(const CreateDesc& desc);
	virtual ~Renderer_Backend() override;

	AX_Renderer_FunctionInterfaces_pure()

	RenderRequest_Backend*		nextRenderRequest();

	virtual void getBindlessTable(BindlessTable<class Sampler_Backend  >* & outList) { outList = nullptr; }
	virtual void getBindlessTable(BindlessTable<class Texture2D_Backend>* & outList) { outList = nullptr; }
	virtual void onUpdateBindlessTables(RenderRequest_Backend* req) {}

	struct Limits {
		Vec2i	maxTexture2DSize {0,0};
	};

	const Limits& limits() const { return _limits; }

	Material_Backend*	commonMaterial();
	Shader_Backend*		commonShader();

	ImFontAtlas* imguiFontAtlas() { return &_imguiFontAtlas; }

	void waitAllRenderCompleted();

	RenderRequest_Backend* getRenderRequest(Int i);

protected:
	virtual void onCreate() override;
	virtual void onDestroy() override;
	virtual void onFileChanged(FileDirWatcher_Result& result) override;

	Limits	_limits;

	ImFontAtlas _imguiFontAtlas;
private:
	struct PrivateData;
	UPtr<PrivateData>	_privateData;
};


} // namespace
