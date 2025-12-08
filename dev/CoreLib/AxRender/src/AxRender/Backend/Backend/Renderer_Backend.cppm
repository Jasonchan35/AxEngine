module;
#include <imgui.h>
export module AxRender:Renderer_Backend;
export import :Renderer;

export namespace ax::AxRender {

class RenderRequest_Backend;

template<class T> class BindlessTable;

#define AX_RenderObject_LIST(E, API, SUFFIX) \
	E(RenderContext				, API, SUFFIX) \
	E(RenderPass				, API, SUFFIX) \
	E(RenderColorBuffer			, API, SUFFIX) \
	E(RenderDepthBuffer			, API, SUFFIX) \
	E(RenderRequest				, API, SUFFIX) \
	E(GpuBuffer					, API, SUFFIX) \
	E(Shader					, API, SUFFIX) \
	E(ShaderParamSpace			, API, SUFFIX) \
	E(Material					, API, SUFFIX) \
	E(MaterialParamSpace		, API, SUFFIX) \
	E(Sampler					, API, SUFFIX) \
	E(Texture2D					, API, SUFFIX) \
//----

#define AX_RenderObject_ForwardDeclare(OBJ, ...)	\
	class OBJ; \
	class OBJ ## _Backend; \
	class OBJ ## _CreateDesc; \
//---
AX_RenderObject_LIST(AX_RenderObject_ForwardDeclare, AX_EMPTY, AX_EMPTY);

#define AX_Renderer_NewObject(OBJ, API, SUFFIX) \
	virtual UPtr<OBJ##_Backend> new##OBJ(const MemAllocRequest& req, const OBJ##_CreateDesc& desc) SUFFIX; \
	virtual void _newObject(UPtr<OBJ##_Backend> & outObj, const MemAllocRequest& req, const OBJ##_CreateDesc& desc) SUFFIX; \
//----
#define AX_Renderer_FunctionInterfaces(API, SUFFIX)	AX_RenderObject_LIST(AX_Renderer_NewObject, API, SUFFIX)

#define AX_Renderer_NewObjectImp(OBJ, API, SUFFIX) \
	UPtr<OBJ##_Backend> Renderer_##API::new##OBJ(const MemAllocRequest& req, const OBJ##_CreateDesc& desc ) { \
		return UPtr_new<OBJ##_##API>(req, desc); \
	} \
	void Renderer_##API::_newObject(UPtr<OBJ##_Backend> & outObj, const MemAllocRequest& req, const OBJ##_CreateDesc& desc) { \
		outObj = UPtr_new<OBJ##_##API>(req, desc); \
	} \
//----
#define AX_Renderer_FunctionBodies(API, SUFFIX)	AX_RenderObject_LIST(AX_Renderer_NewObjectImp, API, SUFFIX)


class Renderer_Backend : public Renderer {
	AX_RTTI_INFO(Renderer_Backend, Renderer)
public:
	AX_DOWNCAST_GET_INSTANCE()

	Renderer_Backend(const CreateDesc& desc);
	virtual ~Renderer_Backend() override;

	AX_Renderer_FunctionInterfaces(AX_EMPTY, =0)

	RenderRequest_Backend*		nextRenderRequest();

	virtual void getBindlessTable(BindlessTable<class Sampler_Backend  >* & outList) { outList = nullptr; }
	virtual void getBindlessTable(BindlessTable<class Texture2D_Backend>* & outList) { outList = nullptr; }
	virtual void getBindlessTable(BindlessTable<class Texture3D_Backend>* & outList) { outList = nullptr; }
	virtual void onUpdateBindlessTables(RenderRequest_Backend* req) {}

	struct Limits {
		Vec2i	maxTexture2DSize {0,0};
	};

	const Limits& limits() const { return _limits; }

		  Material_Backend*	commonMaterial();
	const Shader_Backend*	commonShader();

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
