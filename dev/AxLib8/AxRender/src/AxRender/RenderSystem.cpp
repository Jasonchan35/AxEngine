module AxRender;

import :RenderSystem_Null;

#if AX_RENDERER_VK
	import :RenderSystem_Vk;
#endif

#if AX_RENDERER_DX12
	import :RenderSystem_Dx12;
#endif

namespace ax /*::AxRender*/ {

static RenderSystem* RenderSystem_instance = nullptr;

RenderSystem* RenderSystem::s_instance() {
	return RenderSystem_instance;
}

UPtr<RenderSystem> RenderSystem::s_create(const CreateDesc& desc) {
	UPtr<RenderSystem> o;
	switch (desc.info.api) {
		case RenderAPI::Null:	o = UPtr_new<RenderSystem_Null>(AX_NEW, desc); break;
#if AX_RENDERER_VK
		case RenderAPI::Vk:		o = UPtr_new<RenderSystem_Vk>(AX_NEW, desc); break;
#endif
#if AX_RENDERER_DX12
		case RenderAPI::Dx12:	o = UPtr_new<RenderSystem_Dx12>(AX_NEW, desc); break;
#endif
		default:	throw Error_Undefined(); break;
	}

	o->onCreate();
	return o;
}

RenderSystem::RenderSystem(const CreateDesc& desc)
: _info(desc.info)
, _enableDebugReport(desc.info.enableDebugReport)
, _enableDebugUtils(desc.info.enableDebugUtils)
, _enableDebugMarker(desc.info.enableDebugMarker)
{
	AX_ASSERT(RenderSystem_instance == nullptr);
	RenderSystem_instance = this;

	_startTime = getCurrentTime();
}

RenderSystem::~RenderSystem() {
	AX_ASSERT(RenderSystem_instance == this);
	RenderSystem_instance = nullptr;
}

void RenderSystem::destroy() {
	onDestroy(); 
}

RenderSystemInfo::RenderSystemInfo() {
#if _DEBUG
	constexpr bool isDebug = true;
#else
	constexpr bool isDebug = false;
#endif

#if AX_OS_WINDOWS
	#if AX_RENDERER_VK
		api = RenderAPI::Vk;
	#elif AX_RENDERER_DX12
		api = RenderAPI::DX12;
	#endif
#endif
//	AX_ASSERT(api != RenderApi::None);

	multithread			= true;
	enableDebugReport	= isDebug;
	enableDebugUtils	= isDebug;
	enableDebugMarker	= isDebug;
	vsync				= true;
	//---
	inlineUpload.bufferSize   =  16 * 1024 * 1024;
	inlineUpload.limitPerEach = 128 * 1024;

	shader.maxConstBufferCount = 6;
	shader.maxTextureCount     = 10;
	shader.maxSamplerCount     = 10;
	
	renderPass.maxCount  = 64;
	renderPass.maxColorBufferCount = 8;
	renderPass.maxDepthBufferCount = 1;

	renderRequest.count = 3;
	Int maxMaterialCount              = 8 * 1024;
	renderRequest.maxConstBufferCount = maxMaterialCount * shader.maxConstBufferCount;
	renderRequest.maxTextureCount     = maxMaterialCount * shader.maxTextureCount;
	renderRequest.maxSamplerCount     = 256;
}

} // namespace


