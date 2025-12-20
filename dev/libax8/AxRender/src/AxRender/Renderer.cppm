module;
export module AxRender:Renderer;
export import :RenderObject;
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

struct RendererInfo {
	RendererInfo();

	RenderAPI	api = RenderAPI::None;
	bool multithread		: 1;
	bool enableDebugReport	: 1;
	bool enableDebugUtils	: 1;
	bool enableDebugMarker	: 1;
	bool vsync              : 1;

	Int  renderRequestCount = 0;

	struct InlineUpload {
		Int bufferSize   = 0;
		Int limitPerEach = 0;
	} inlineUpload;
};

struct Renderer_CreateDesc {
	RendererInfo	info;
};

class Renderer : public RenderObject {
	AX_RTTI_INFO(Renderer, RenderObject)
public:
	using CreateDesc = Renderer_CreateDesc;

	static Renderer*		s_instance();
	static UPtr<Renderer>	s_create(const CreateDesc& desc);

	Renderer(const CreateDesc& desc);
	virtual ~Renderer() override;

	RenderAPI	api() const			{ return _info.api; }
	bool		multithread() const	{ return _info.multithread; }
	bool		vsync() const		{ return _info.vsync; }

	struct AdapterInfo {
		String	name;
		Int		memorySize = 0;
		bool	multithread = false;
		bool	computeShader = false;
		bool	shaderFloat64 = false;
		Int		minThreadGroupSize = 1;
	};

	const AdapterInfo&		adapterInfo() const { return _adapterInfo; }
	
	using MemoryInfo = RenderMemoryInfo;
	void getMemoryInfo(MemoryInfo& info) { onGetMemoryInfo(info); }

	bool enableDebugReport() const { return _enableDebugReport; }

//	AX_INLINE RenderSeqId	renderSeqId() const { return _renderSeqId; }
	AX_INLINE const RendererInfo&	info() const { return _info; }

	AX_INLINE Int renderRequestCount() const { return _info.renderRequestCount; }

	const HiResTime&	startTime() const { return _startTime; }

	HiResTime	getCurrentTime() const { return HiResTime::s_now(); }
	HiResTime	getCurrentUptime() const { return HiResTime::s_now() - _startTime; }

	virtual void onFileChanged(FileDirWatcher_Result& result) {}

	Int copyGpuBufferAlignment() const { return _copyGpuBufferAlignment; }
	
friend class RenderContext_Backend;
protected:
	void destroy();
	virtual void onCreate() {}
	virtual void onDestroy() {}

	virtual void onGetMemoryInfo(MemoryInfo& info) { info = {}; }
	AdapterInfo			_adapterInfo;
	RenderSeqId			_renderSeqId = 0;
	HiResTime			_startTime;
	Int					_copyGpuBufferAlignment = 16;

	RendererInfo		_info;

	bool _enableDebugReport : 1;
	bool _enableDebugUtils  : 1;
	bool _enableDebugMarker : 1;
};


} // namespace

