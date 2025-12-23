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

	AX_Renderer_FunctionInterfaces(AX_EMPTY, =0)

	RenderRequest_Backend*		nextRenderRequest();
	
	Material_Backend*			commonMaterial();
	MaterialPass_Backend*		commonMaterialPass();
	
	Shader_Backend*				commonShader();
	ShaderPass_Backend*			commonShaderPass();

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
