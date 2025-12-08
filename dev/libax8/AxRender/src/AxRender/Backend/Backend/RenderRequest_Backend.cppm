module;
export module AxRender:RenderRequest_Backend;
export import :Renderer;
export import :RenderRequest;
export import :RenderContext_Backend;
export import :GpuBuffer_Backend;
export import :Texture_Backend;
export import :Material_Backend;
export import :RenderPass_Backend;

export namespace ax::AxRender {

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
	void waitCompletedAndReset(RenderSeqId renderSeqId);

	void frameBegin(RenderContext_Backend* renderContext, RenderPass_Backend* backBufferRenderPass);
	void frameEnd();

	void renderPassBegin(RenderPass_Backend* pass);
	void renderPassEnd();

	void setCurrentRenderPass(RenderPass* pass) { _currentRenderPass = pass; }

	virtual void onDrawcall(Cmd_DrawCall& cmd) override;

	RenderPass_Backend*	backBufferRenderPass()	{ return _backBufferRenderPass; }

	class Renderer_Backend* renderer() { return rttiCastCheck<Renderer_Backend>(_renderer); }
	class RenderContext_Backend* renderContext() { return rttiCastCheck<RenderContext_Backend>(_renderContext); }

//	CommandBuffer&	cmdBuf() { return *_cmdBuf; }

	struct ResourcesList {
		void add(GpuBuffer_Backend*		p) {   gpuBuffers.emplaceBack(p); }
		void add(Material_Backend*		p) {    materials.emplaceBack(p); }
		void add(RenderPass_Backend*	p) { renderPasses.emplaceBack(p); }
		void add(Sampler_Backend*		p) {     samplers.emplaceBack(p); }
		void add(Texture2D_Backend*		p) {   texture2Ds.emplaceBack(p); }
		void add(Texture3D_Backend*		p) {   texture3Ds.emplaceBack(p); }
		void add(TextureCube_Backend*	p) { textureCubes.emplaceBack(p); }

		void add(SPtr<GpuBuffer_Backend  > && p) {   gpuBuffers.emplaceBack(std::move(p)); }
		void add(SPtr<Material_Backend   > && p) {    materials.emplaceBack(std::move(p)); }
		void add(SPtr<RenderPass_Backend > && p) { renderPasses.emplaceBack(std::move(p)); }
		void add(SPtr<Sampler_Backend    > && p) {     samplers.emplaceBack(std::move(p)); }
		void add(SPtr<Texture2D_Backend  > && p) {   texture2Ds.emplaceBack(std::move(p)); }
		void add(SPtr<Texture3D_Backend  > && p) {   texture3Ds.emplaceBack(std::move(p)); }
		void add(SPtr<TextureCube_Backend> && p) { textureCubes.emplaceBack(std::move(p)); }

		void clear() {
			gpuBuffers.clear();
			materials.clear();
			renderPasses.clear();
			samplers.clear();
			texture2Ds.clear();
			texture3Ds.clear();
		}

		Array< SPtr<GpuBuffer_Backend  > >	gpuBuffers;
		Array< SPtr<Material_Backend   > >	materials;
		Array< SPtr<RenderPass_Backend > >	renderPasses;
		Array< SPtr<Sampler_Backend    > >	samplers;
		Array< SPtr<Texture2D_Backend  > >	texture2Ds;
		Array< SPtr<Texture3D_Backend  > >	texture3Ds;
		Array< SPtr<TextureCube_Backend> >	textureCubes;
	};

	ResourcesList	resourcesToKeep;
	ResourcesList	resourcesToUpdateDescriptor;

	// TODO: use circular-buffer and move to renderer
	struct InlineUpload {
		InlineUpload() { reset(); }

		bool copyDataToGpuBuffer(GpuBuffer_Backend* dst, ByteSpan data, Int dstOffset);

		Int remainSize() const	{ return bufferSize() - _used; }
		Int bufferSize() const	{ return _gpuBuffer->bufferSize(); }

	friend class RenderRequest_Backend;
	protected:
		void create(RenderRequest_Backend* req);
		void reset() { _used = 0; }

	private:
		RenderRequest*	_req = nullptr;
		Int				_used = 0;
		Int				_limitPerEach = 0;
		SPtr<GpuBuffer_Backend>	_gpuBuffer;
	};

	InlineUpload	inlineUpload;

	Int index() const { return _index; }

protected:
	virtual void onDrawUI() override;

	virtual void onWaitCompleted() {}
	virtual void onFrameBegin() {}
	virtual void onFrameEnd() {}

	virtual void onRenderPassBegin() {}
	virtual void onRenderPassEnd() {}

private:
	Int _index = 0;
	SPtr<RenderPass_Backend>	_backBufferRenderPass;

};

} // namespace
