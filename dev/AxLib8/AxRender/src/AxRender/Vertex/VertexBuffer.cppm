module;
export module AxRender:VertexBuffer;
export import :GpuBuffer;
export import :VertexLayout;

export namespace ax /*::AxRender*/ {

class VertexBuffer : public NonCopyable {
public:
	void clear() { _vertexCount = 0; _buffer.reset(); }

	AX_INLINE Int			vertexCount() const		{ return _vertexCount; }
	AX_INLINE VertexLayout	vertexLayout() const	{ return _vertexLayout; }

							void create(VertexLayout vertexLayout);
	template<class VERTEX>	void create() { create(VERTEX::s_layout()); }
	template<class VERTEX>	void create(Span<VERTEX> data) { create<VERTEX>(); addVertices(data); }

	template<class VERTEX>
	void addVertices(Span<VERTEX> data) { addVertices(data.toByteSpan(), VERTEX::s_layout()); }
	void addVertices(ByteSpan   data, VertexLayout vertexLayout);

	void ensureBufferCapacity(Int n) { _buffer.ensureDataCapacity(n * _vertexLayout->strideInBytes); }

	AX_INLINE const GpuBuffer* getUploadedGpuBuffer(class RenderRequest* req) const {
		return _buffer.getUploadedGpuBuffer(req);
	}
	
private:
	VertexLayout		_vertexLayout;
	Int					_vertexCount = 0;
	DynamicGpuBuffer	_buffer;
};

class IndexBuffer : public NonCopyable {
public:
	void clear() { _indexCount = 0; _buffer.reset(); }

	AX_INLINE Int			indexCount() const		{ return _indexCount; }
	AX_INLINE IndexType		indexType() const		{ return _indexType; }

							void create(IndexType indexType);
	template<class INDEX>	void create() { create(IndexType_get<INDEX>); }
	template<class INDEX>	void create(Span<INDEX> data) { create<INDEX>(); addVertices(data); }

	template<class INDEX>
	void addIndices(Span<INDEX> data) { addIndices(data.toByteSpan(), IndexType_get<INDEX>); }
	void addIndices(ByteSpan data, IndexType indexType);

	void ensureBufferCapacity(Int n) { _buffer.ensureDataCapacity(n * IndexType_stride(_indexType)); }

	AX_INLINE const GpuBuffer* getUploadedGpuBuffer(RenderRequest* req) const {
		return _buffer.getUploadedGpuBuffer(req);
	}

private:
	IndexType			_indexType = IndexType::None;
	Int					_indexCount = 0;
	DynamicGpuBuffer	_buffer;
};

//---------

AX_INLINE
void VertexBuffer::addVertices(ByteSpan data, VertexLayout vertexLayout) {
	if (!vertexLayout)
		throw Error_Undefined();

	if (!_vertexLayout) {
		create(vertexLayout);
	} else if (_vertexLayout != vertexLayout) {
		throw Error_Undefined();
	}

	const Int stride = vertexLayout->strideInBytes;
	if (data.size() % stride) AX_ASSERT(false);

	Int newVertexCount = data.size() / stride;
	Int newDataBytes   = newVertexCount * stride;

	_buffer.appendData(data.slice(0, newDataBytes));
	_vertexCount += newVertexCount;
}

AX_INLINE
void IndexBuffer::addIndices(ByteSpan data, IndexType indexType) {
	if (indexType == IndexType::None)
		throw Error_Undefined();

	if (_indexType == IndexType::None) {
		create(indexType);
	} else if (_indexType != indexType) {
		throw Error_Undefined();
	}

	const Int stride = IndexType_stride(indexType);
	if (data.size() % stride) AX_ASSERT(false);

	Int newIndexCount = data.size() / stride;
	Int newDataBytes   = newIndexCount * stride;

	_buffer.appendData(data.slice(0, newDataBytes));
	_buffer.markDirty(Range_StartAndSize(_indexCount * stride, newDataBytes));
	_indexCount += newIndexCount;
}


} // namespace
