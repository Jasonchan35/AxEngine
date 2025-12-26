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
	RenderSystem_Backend* renderSystem = nullptr;
	Int index = 0;
};

class RenderRequest_Backend : public RenderRequest {
	AX_RTTI_INFO(RenderRequest_Backend, RenderRequest)
public:
	using CreateDesc = RenderRequest_CreateDesc;

	static UPtr<This> s_new(const MemAllocRequest& req, RenderSystem_Backend* renderSystem, Int index);

	RenderRequest_Backend(const CreateDesc& desc);

	void waitCompleted();
	void waitCompletedAndReset(RenderSeqId newRenderSeqId);

	void frameBegin(RenderContext_Backend* renderContext, RenderPass_Backend* backBufferRenderPass);
	void frameEnd();

	void renderPassBegin(RenderPass_Backend* pass);
	void renderPassEnd(RenderPass_Backend* pass);

	void setCurrentRenderPass(RenderPass* pass) { _currentRenderPass = pass; }

	RenderPass_Backend*	backBufferRenderPass()	{ return _backBufferRenderPass; }

	class RenderSystem_Backend* renderSystem_backend() { return rttiCastCheck<RenderSystem_Backend>(_renderSystem); }
	class RenderContext_Backend* renderContext_backend() { return rttiCastCheck<RenderContext_Backend>(_renderContext); }

#if AX_RENDER_BINDLESS
	struct UpdatedBindlessResources {
		Array<SPtr<Sampler_Backend  >>	samplers;
		Array<SPtr<Texture2D_Backend>>	texture2Ds;
		Array<SPtr<StorageBuffer_Backend>>	storageBuffers;

		void swap(Array<SPtr<Sampler_Backend>> r) { std::swap(r, samplers); }
		void swap(Array<SPtr<Texture2D_Backend>> r) { std::swap(r, texture2Ds); }
		void swap(Array<SPtr<StorageBuffer_Backend>> r) { std::swap(r, storageBuffers); }

		void clear() {
			samplers.clear();
			texture2Ds.clear();
			storageBuffers.clear();
		}
	};

	UpdatedBindlessResources updatedBindlessResources;
#endif	
	
	struct ResourcesList {

		void add(const  GpuBuffer_Backend* p) { _add(p); }
		void add(const   Material_Backend* p) { _add(p); }
		void add(const RenderPass_Backend* p) { _add(p); }
		void add(const    Sampler_Backend* p) { _add(p); }
		void add(const  Texture2D_Backend* p) { _add(p); }

		void clear() {
			visit([](auto& list) { list.clear(); });
		}

		template<class FUNC>
		void visit(FUNC func) {
			_all_list.apply([&func](auto&... list) {
				(func(list),...);
			});
		}
		
		template<class T> Array<SPtr<const T>>& getList() { return _all_list.get< Array<SPtr<const T>> >(); }

	private:
		template<class T> void _add(     const T  *  p) { getList<const T>().emplaceBack(p); }
		template<class T> void _add(SPtr<const T> && p) { getList<const T>().emplaceBack(std::move(p)); }
		
		using All_List = Tuple<
			Array< SPtr<const  GpuBuffer_Backend> >,
			Array< SPtr<const   Material_Backend> >,
			Array< SPtr<const RenderPass_Backend> >,
			Array< SPtr<const    Sampler_Backend> >,
			Array< SPtr<const  Texture2D_Backend> >
		>;
		All_List _all_list;
	};

	ResourcesList	resourcesToKeep;

	void copyDataToGpuBuffer(GpuBuffer* dst, ByteSpan data, Int dstOffset);
	bool copyDataToGpuBuffer_InlineBuffer(GpuBuffer* dst, ByteSpan data, Int dstOffset);
	void copyDataToGpuBuffer_StagingBuffer(GpuBuffer* dst, ByteSpan data, Int dstOffset);
	

	Int index() const { return _index; }

	void drawUI_backend();
	void drawCall_backend(Cmd_DrawCall& cmd);
	void setViewport_backend(const Rect2f& rect, float minDepth, float maxDepth);
	void setScissorRect_backend(const Rect2f& rect);

	Material_Backend*		commonMaterial()		{ return _commonMaterial; };
	MaterialPass_Backend*	commonMaterialPass()	{ return _commonMaterialPass; };

protected:
	AX_RenderRequest_Backend_FunctionInterfaces(=0)

	RenderSystem_Backend* _renderSystem_backend = nullptr;
	Material_Backend*     _commonMaterial       = nullptr;
	MaterialPass_Backend* _commonMaterialPass   = nullptr;

private:
	void _updateCommonMaterial();
	
	Int _index = 0;
	SPtr<RenderPass_Backend>	_backBufferRenderPass;

	struct InlineUpload {
		Int remainSize() const	{ return bufferSize() - usedBytes; }
		Int bufferSize() const	{ return stagingToGpuBuffer->bufferSize(); }
		void create(RenderRequest_Backend* req);

		void reset() { usedBytes = 0; }
		Int	usedBytes = 0;
		Int	limitPerEach = 0;
		SPtr<GpuBuffer_Backend>	stagingToGpuBuffer;
	};

	InlineUpload _inlineUpload;
};

template<class OWNER, class DATA, class...ARGS>
struct RenderPerFrameDataSet_ {
	RenderPerFrameDataSet_(OWNER* owner_) : _owner(owner_) {}
	virtual ~RenderPerFrameDataSet_() = default;
	
	static constexpr auto kCount = AxRenderConfig::kMaxRenderRequestCount;
	using Data = DATA;

	const DATA& getUpdated(RenderRequest_Backend* req, ARGS&&... ages) const {
		return ax_const_cast(this)->_getUpdated(req, AX_FORWARD(ages)...);
	}

	// callback to owner
	// void onUpdatePerFrameData(Int currentIndex, RenderRequest_Backend* req, DATA& data, ARGS&&... ages);
	
private:

	DATA& _current() { return _perFrameData[_currentIndex]; }
	DATA& _getUpdated(RenderRequest_Backend* req, ARGS&&... ages) {
		if (_renderSeqIdGraud.update(req)) {
			_currentIndex = (_currentIndex + 1) % _perFrameData.size();
			_owner->onUpdatePerFrameData(_currentIndex, req, _current(), AX_FORWARD(ages)...);
		}
		return _current();
	}

	OWNER*                   _owner = nullptr;
	RenderSeqIdGraud         _renderSeqIdGraud;
	Int                      _currentIndex = 0;
	FixedArray<DATA, kCount> _perFrameData;
};


inline
void RenderRequest_Backend::copyDataToGpuBuffer(GpuBuffer* dst, ByteSpan data, Int dstOffset) {
	if (copyDataToGpuBuffer_InlineBuffer(dst, data, dstOffset)) {
		return;
	}
	copyDataToGpuBuffer_StagingBuffer(dst, data, dstOffset);
}


} // namespace
