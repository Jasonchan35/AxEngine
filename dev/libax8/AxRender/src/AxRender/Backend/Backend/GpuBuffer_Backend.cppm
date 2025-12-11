module;
export module AxRender:GpuBuffer_Backend;
export import :Renderer_Backend;
export import :GpuBuffer;

export namespace ax::AxRender {

class GpuBuffer_Backend : public GpuBuffer {
	AX_RTTI_INFO(GpuBuffer_Backend, GpuBuffer)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView name, GpuBufferType type, Int size) {
		return s_new(req, CreateDesc(name, type, size));
	}

	void copyData(ByteSpan data, Int offset = 0);

	void copyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) {
		onCopyFromGpuBuffer(req, src, srcRange, dstOffset);
	}

	struct MapScope : public NonCopyable {
		AX_NODISCARD MapScope(This* obj, MutByteSpan data) 
			: _obj(obj), _data(data) {}
		~MapScope() { if (_obj) _obj->unmapMemory(); }

		MutByteSpan* operator->() { return &_data; }
		MutByteSpan	data() { return _data; }

	private:
		This*			_obj = nullptr;
		MutByteSpan		_data;
	};

	AX_NODISCARD MapScope mapMemory() { return mapMemory(IntRange(bufferSize())); }
	AX_NODISCARD MapScope mapMemory(IntRange range);

	void flush(IntRange range);

protected:

	void unmapMemory() { onUnmapMemory(); }

	virtual MutByteSpan	onMapMemory(IntRange range) = 0;
	virtual void		onUnmapMemory() = 0;
	virtual void		onFlush(IntRange range) = 0;
	virtual void		onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) = 0;


	GpuBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
};

class RenderColorBuffer_Backend : public RenderColorBuffer {
	AX_RTTI_INFO(RenderColorBuffer_Backend, RenderColorBuffer)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

protected:
	RenderColorBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
};

class RenderDepthBuffer_Backend : public RenderDepthBuffer {
	AX_RTTI_INFO(RenderDepthBuffer_Backend, RenderDepthBuffer)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

protected:
	RenderDepthBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
};

} // namespace
