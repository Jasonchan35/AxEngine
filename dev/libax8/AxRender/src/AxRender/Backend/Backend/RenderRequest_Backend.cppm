module;
export module AxRender:RenderRequest_Backend;
export import :Renderer;
export import :RenderRequest;
export import :RenderContext_Backend;
export import :GpuBuffer_Backend;
export import :Texture_Backend;
export import :Material_Backend;
export import :RenderPass_Backend;

export namespace ax /*::AxRender*/ {

class RenderRequest_CreateDesc : public NonCopyable {
public:
	Renderer*	renderer = nullptr;
	Int index = 0;
};

class RenderRequest_Backend : public RenderRequest {
	AX_RTTI_INFO(RenderRequest_Backend, RenderRequest)
public:
	using CreateDesc = RenderRequest_CreateDesc;

	static UPtr<This> s_new(const MemAllocRequest& req, Renderer* renderer, Int index);

	RenderRequest_Backend(const CreateDesc& desc);

	void waitCompleted();
	void waitCompletedAndReset(RenderSeqId newRenderSeqId);

	void frameBegin(RenderContext_Backend* renderContext, RenderPass_Backend* backBufferRenderPass);
	void frameEnd();

	void renderPassBegin(RenderPass_Backend* pass);
	void renderPassEnd(RenderPass_Backend* pass);

	void setCurrentRenderPass(RenderPass* pass) { _currentRenderPass = pass; }

	RenderPass_Backend*	backBufferRenderPass()	{ return _backBufferRenderPass; }

	class Renderer_Backend* renderer_backend() { return rttiCastCheck<Renderer_Backend>(_renderer); }
	class RenderContext_Backend* renderContext_backend() { return rttiCastCheck<RenderContext_Backend>(_renderContext); }

//	CommandBuffer&	cmdBuf() { return *_cmdBuf; }

	struct ResourcesList {
		template<class T> Array<SPtr<T>>& getList() { return _all_list.get< Array<SPtr<T>> >(); }

		void add(  GpuBuffer_Backend* p) { _add(p); }
		void add(   Material_Backend* p) { _add(p); }
		void add( RenderPass_Backend* p) { _add(p); }
		void add(    Sampler_Backend* p) { _add(p); }
		void add(  Texture2D_Backend* p) { _add(p); }

		void add(SPtr<  GpuBuffer_Backend> && p) { _add(AX_FORWARD(p)); }
		void add(SPtr<   Material_Backend> && p) { _add(AX_FORWARD(p)); }
		void add(SPtr< RenderPass_Backend> && p) { _add(AX_FORWARD(p)); }
		void add(SPtr<    Sampler_Backend> && p) { _add(AX_FORWARD(p)); }
		void add(SPtr<  Texture2D_Backend> && p) { _add(AX_FORWARD(p)); }

		void clear() {
			_all_list.apply([](auto&... list) {
				(list.clear(),...);
			});
		}

	private:
		template<class T> void _add(     T  *  p) { getList<T>().emplaceBack(p); }
		template<class T> void _add(SPtr<T> && p) { getList<T>().emplaceBack(std::move(p)); }
		
		using All_List = Tuple<
			Array< SPtr<  GpuBuffer_Backend> >,
			Array< SPtr<   Material_Backend> >,
			Array< SPtr< RenderPass_Backend> >,
			Array< SPtr<    Sampler_Backend> >,
			Array< SPtr<  Texture2D_Backend> >
		>;
		All_List _all_list;
	};

	ResourcesList	resourcesToKeep;
	ResourcesList	resourcesToUpdateDescriptor;

	struct InlineUpload {
		InlineUpload() { reset(); }

		bool tryCopyDataToGpuBuffer(GpuBuffer* dst, ByteSpan data, Int dstOffset);

		Int remainSize() const	{ return bufferSize() - _used; }
		Int bufferSize() const	{ return _stagingToGpuBuffer->bufferSize(); }

	friend class RenderRequest_Backend;
	protected:
		void create(RenderRequest_Backend* req);
		void reset() { _used = 0; }

	private:
		RenderRequest*	_req = nullptr;
		Int				_used = 0;
		Int				_limitPerEach = 0;
		SPtr<GpuBuffer_Backend>	_stagingToGpuBuffer;
	};

	InlineUpload	inlineUpload;

	Int index() const { return _index; }

	void drawUI_backend();
	void drawCall_backend(Cmd_DrawCall& cmd);
	void setViewport_backend(const Rect2f& rect, float minDepth, float maxDepth);
	void setScissorRect_backend(const Rect2f& rect);
	
protected:
	AX_RenderRequest_Backend_FunctionInterfaces(=0)
	
private:
	void _updateCommonMaterial();
	
	Int _index = 0;
	SPtr<RenderPass_Backend>	_backBufferRenderPass;
};

} // namespace
