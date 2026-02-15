module;
export module AxRender:RenderSystem;
export import :RenderDataType;

export namespace ax /*::AxRender*/ {

#define AX_RenderAPI_ENUM_LIST(E) \
	E(None,) \
	E(Null,) \
	E(Dx12,) \
	E(Vk,) \
//-----
AX_ENUM_CLASS(AX_RenderAPI_ENUM_LIST, RenderAPI, u8);

class GpuUploadManager;

struct RenderSystemInfo {
	RenderSystemInfo();

	RenderAPI	api = RenderAPI::None;
	bool multithread		: 1;
	bool vsync              : 1;
	
	bool enableDebugReport	: 1;
	bool enableDebugUtils	: 1;
	bool enableDebugMarker	: 1;

	struct IndirectDraw {
		Int maxDrawCount = 0;
	} indirectDraw;

	struct InlineUpload {
		Int maxBufferSize    = 0;
		Int maxSizePerUpload = 0;
	} inlineUpload;

	struct Shader {
		Int maxConstBufferCount = 0;
		Int maxTextureCount     = 0;
		Int maxSamplerCount     = 0;
	} shader;
	
	struct RenderPass {
		Int maxCount = 0;
		Int maxColorBufferCount = 0;
		Int maxDepthBufferCount = 0;
	} renderPass;
	
	struct RenderRequest {
		Int count = 0;
		Int maxConstBufferCount = 0;
		Int maxTextureCount     = 0;
		Int maxSamplerCount     = 0;
	} renderRequest;
};

struct RenderSystem_CreateDesc {
	RenderSystemInfo	info;
};

class RenderSystem : public RenderObject {
	AX_RTTI_INFO(RenderSystem, RenderObject)
public:
	using CreateDesc = RenderSystem_CreateDesc;

	static RenderSystem*		s_instance();
	static UPtr<RenderSystem>	s_create(const CreateDesc& desc);

	RenderSystem(const CreateDesc& desc);
	virtual ~RenderSystem() override;

	RenderAPI	api() const			{ return _info.api; }
	bool		multithread() const	{ return _info.multithread; }
	bool		vsync() const		{ return _info.vsync; }

	struct AdapterInfo {
		String name;
		Int    memorySize                      = 0;
		bool   multithread                     = false;
		bool   computeShader                   = false;
		bool   shaderFloat64                   = false;
		Int    minThreadGroupSize              = 1;
		Vec1i  maxTexture1DSize                = {0};
		Vec2i  maxTexture2DSize                = {0, 0};
		Vec3i  maxTexture3DSize                = {0, 0, 0};
		Int    minMemoryMapAlignment           = 1;
		Int    minUniformBufferOffsetAlignment = 1;
	};

	const AdapterInfo&		adapterInfo() const { return _adapterInfo; }
	
	using MemoryInfo = RenderMemoryInfo;
	void getMemoryInfo(MemoryInfo& info) { onGetMemoryInfo(info); }

	bool enableDebugReport() const { return _enableDebugReport; }

//	AX_INLINE RenderSeqId	renderSeqId() const { return _renderSeqId; }
	AX_INLINE const RenderSystemInfo&	info() const { return _info; }

	AX_INLINE Int renderRequestCount() const { return _info.renderRequest.count; }

	const HiResTime&	startTime() const { return _startTime; }

	HiResTime	getCurrentTime() const		{ return HiResTime::s_now(); }
	HiResTime	getCurrentUptime() const	{ return HiResTime::s_now() - _startTime; }

	virtual void onFileChanged(FileDirWatcher_Result& result) {}
	
friend class RenderContext_Backend;
protected:
	void destroy();
	virtual void onCreate() {}
	virtual void onDestroy() {}

	virtual void onGetMemoryInfo(MemoryInfo& info) { info = {}; }
	AdapterInfo			_adapterInfo;
	RenderSeqId			_renderSeqId = 0;
	HiResTime			_startTime;

	RenderSystemInfo		_info;

#if AX_RENDER_DEBUG_LAYER
	bool _enableDebugReport : 1;
	bool _enableDebugUtils  : 1;
	bool _enableDebugMarker : 1;
#endif
};


} // namespace

