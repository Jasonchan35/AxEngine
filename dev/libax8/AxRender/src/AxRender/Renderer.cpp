module AxRender;

import :Renderer_Null;

#if AX_RENDERER_VK
	import :Renderer_VK;
#endif

namespace ax::AxRender {

static Renderer* Renderer_instance = nullptr;

Renderer* Renderer::s_instance() {
	return Renderer_instance;
}

UPtr<Renderer> Renderer::s_create(const CreateDesc& desc) {
	UPtr<Renderer> o;
	switch (desc.info.api) {
		case RenderApi::Null:	o = UPtr_new<Renderer_Null>(AX_ALLOC_REQ, desc); break;
#if AX_RENDERER_VK
		case RenderApi::VK:		o = UPtr_new<Renderer_VK>(AX_ALLOC_REQ, desc); break;
#endif
#if AX_RENDERER_DX12
		case RenderApi::DX12:	o = UPtr_new<Renderer_DX12>(AX_ALLOC_REQ, desc); break;
#endif
		default:	throw Error_Undefined(); break;
	}

	o->onCreate();
	return o;
}

Renderer::Renderer(const CreateDesc& desc)
: _info(desc.info)
, _enableDebugReport(desc.info.enableDebugReport)
, _enableDebugUtils(desc.info.enableDebugUtils)
, _enableDebugMarker(desc.info.enableDebugMarker)
{
	AX_ASSERT(Renderer_instance == nullptr);
	Renderer_instance = this;

	_startTime = getCurrentTime();
}

Renderer::~Renderer() {
	AX_ASSERT(Renderer_instance == this);
	Renderer_instance = nullptr;
}

void Renderer::destroy() {
	onDestroy(); 
}

RendererInfo::RendererInfo() {
#if _DEBUG
	const bool isDebug = true;
#else
	const bool isDebug = false;
#endif

#if AX_OS_WINDOWS
	#if AX_RENDERER_VK
		api = RenderApi::VK;
	#elif AX_RENDERER_DX12
		api = RenderApi::DX12;
	#endif
#endif
//	AX_ASSERT(api != RenderApi::None);

	multithread			= true;
	enableDebugReport	= isDebug;
	enableDebugUtils	= isDebug;
	enableDebugMarker	= isDebug;
	//---
	renderRequestCount = 4;
	inlineUpload.bufferSize   =  16 * 1024 * 1024;
	inlineUpload.limitPerEach = 128 * 1024;
}

} // namespace


