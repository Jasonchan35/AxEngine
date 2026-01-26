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
	
	Int strideInBytes() const { return _vertexLayout->strideInBytes; }

	void ensureBufferCapacity(Int n) { _buffer.ensureDataCapacity(n * strideInBytes()); }

	void create(VertexLayout vertexLayout, const GpuVirtualMemoryDesc& virMemDesc = {});
	
	template<class VERTEX>
	void create(const GpuVirtualMemoryDesc& virMemDesc = {}) {
		create(VERTEX::s_layout(), virMemDesc);
	}
	template<class VERTEX>
	void create(Span<VERTEX> data, const GpuVirtualMemoryDesc& virMemDesc = {}) {
		create<VERTEX>(virMemDesc); addVertices(data);
	}

	template<class VERTEX>
	void addVertices(Span<VERTEX> data) { addVertices(data.toByteSpan(), VERTEX::s_layout()); }
	void addVertices(ByteSpan data, VertexLayout vertexLayout);
	
	template<class VERTEX>
	MutSpan<VERTEX> editNewVertices(Int vertexCount) {
		if (VERTEX::s_layout() != _vertexLayout) throw Error_Undefined();
		return MutSpan<VERTEX>::s_fromByteSpan(editNewVertices(vertexCount, VERTEX::s_layout));
	}
	MutByteSpan editNewVertices(Int n, VertexLayout vertexLayout);

	template<class T> using ElemEnumerator_ = Span_InterleaveForEach_<T>;
	
	bool hasElement(VertexSemantic sem) const { return _vertexLayout->find(sem) != nullptr; }

	template<class E> ElemEnumerator_<E> editElements(VertexSemantic sem, IntRange vertexRange);
	template<class E> ElemEnumerator_<E> editElements(const VertexLayoutElement* elem, IntRange vertexRange);
	template<class E> Opt<ElemEnumerator_<E>> tryEditElements(VertexSemantic semantic, IntRange vertexRange) {
		auto* elem = _vertexLayout->find(semantic);
		if (!elem) return std::nullopt;
		if (elem->dataType != RenderDataType_get<E>) {
			AX_ASSERT(false);
			return std::nullopt;
		}
		return editElements<E>(semantic, vertexRange);
	}
	template<class E> Opt<ElemEnumerator_<E>> tryEditElements(VertexSemantic semantic) {
		return tryEditElements<E>(semantic, IntRange(_vertexCount));
	}

	void markDirty(IntRange vertexRange);

	AX_INLINE const GpuBuffer* getUploadedGpuBuffer(class RenderRequest* req) const {
		return _buffer.getUploadedGpuBuffer(req);
	}
	
	bool isEmpty() const { return _vertexCount <= 0; }

private:
	VertexLayout		_vertexLayout;
	Int					_vertexCount = 0;
	DynamicGpuBuffer	_buffer;
};

template<class E> inline
VertexBuffer::ElemEnumerator_<E> VertexBuffer::editElements(VertexSemantic sem, IntRange vertexRange) {
	auto* elem = _vertexLayout->find(sem);
	if (!elem) throw Error_Undefined();
	return editElements<E>(elem, vertexRange);
}

template<class E> inline
VertexBuffer::ElemEnumerator_<E> VertexBuffer::editElements(const VertexLayoutElement* elem, IntRange vertexRange) {
	if (elem->dataType != RenderDataType_get<E>) throw Error_Undefined();
	if (!IntRange(_vertexCount).contains(vertexRange))  throw Error_Undefined();

	auto rangeInBytes = vertexRange * _vertexLayout->strideInBytes;
	_buffer.markDirty(rangeInBytes);
	auto mutData = _buffer.mutSpan().slice(rangeInBytes);
	return ElemEnumerator_<E>(	reinterpret_cast<E*>(mutData.begin() + elem->offset), 
								reinterpret_cast<E*>(mutData.end()   + elem->offset), 
								strideInBytes());
}

class VertexIndexBuffer : public NonCopyable {
public:
	void clear() { _indexCount = 0; _buffer.reset(); }

	AX_INLINE Int				indexCount() const		{ return _indexCount; }
	AX_INLINE VertexIndexType	indexType() const		{ return _indexType; }

	void create(VertexIndexType indexType, const GpuVirtualMemoryDesc& virMemDesc = {});
	
	template<class INDEX>
	void create(const GpuVirtualMemoryDesc& virMemDesc = {}) {
		create(VertexIndexType_get<INDEX>, virMemDesc);
	}
	
	template<class INDEX>
	void create(Span<INDEX> data, const GpuVirtualMemoryDesc& virMemDesc = {}) {
		create<INDEX>(virMemDesc); addVertices(data);
	}

	template<class INDEX>
	void addIndices(Span<INDEX> data) { addIndices(data.toByteSpan(), VertexIndexType_get<INDEX>); }
	void addIndices(ByteSpan data, VertexIndexType indexType);

	template<class INDEX>
	MutSpan<INDEX> editNewIndices(Int indexCount) {
		if (VertexIndexType_get<INDEX> != _indexType) throw Error_Undefined();
		return MutSpan<INDEX>::s_fromMutByteSpan(_editNewIndices(indexCount));
	}
	
	MutByteSpan _editNewIndices(Int n) {
		auto mutSpan = _buffer.extendSize(n * sizeofIndex());
		_indexCount += n;
		return mutSpan;
	}
	
	void ensureBufferCapacity(Int n) { _buffer.ensureDataCapacity(n * VertexIndexType_stride(_indexType)); }

	AX_INLINE const GpuBuffer* getUploadedGpuBuffer(RenderRequest* req) const {
		if (_indexType == VertexIndexType::None) return nullptr;
		return _buffer.getUploadedGpuBuffer(req);
	}

	bool isEmpty() const { return _indexCount <= 0; }
	Int	sizeofIndex		() const { return VertexIndexType_sizeInBytes(_indexType); }
	Int	indexLimit		() const { return VertexIndexType_limit(_indexType); }
	
private:
	VertexIndexType		_indexType = VertexIndexType::None;
	Int					_indexCount = 0;
	DynamicGpuBuffer	_buffer;
};

//---------

AX_INLINE
void VertexBuffer::addVertices(ByteSpan data, VertexLayout vertexLayout) {
	auto newVertexCount = data.sizeInBytes() / vertexLayout->strideInBytes;
	auto mutSpan = editNewVertices(newVertexCount, vertexLayout);
	mutSpan.copyValues(data);
}

MutByteSpan VertexBuffer::editNewVertices(Int n, VertexLayout vertexLayout) {
	if (!_vertexLayout) {
		create(vertexLayout);
	} else if (_vertexLayout != vertexLayout) {
		throw Error_Undefined();
	}
	auto mutSpan = _buffer.extendSize(n * _vertexLayout->strideInBytes);
	_vertexCount += n;
	return mutSpan;
}

inline
void VertexBuffer::markDirty(IntRange vertexRange) {
	auto rangeInBytes = vertexRange * _vertexLayout->strideInBytes;
	_buffer.markDirty(rangeInBytes);
}

AX_INLINE
void VertexIndexBuffer::addIndices(ByteSpan data, VertexIndexType indexType) {
	if (indexType == VertexIndexType::None)
		throw Error_Undefined();

	if (_indexType == VertexIndexType::None) {
		create(indexType);
	} else if (_indexType != indexType) {
		throw Error_Undefined();
	}

	const Int stride = VertexIndexType_stride(indexType);
	if (data.size() % stride) AX_ASSERT(false);

	Int newIndexCount = data.size() / stride;
	Int newDataBytes   = newIndexCount * stride;

	_buffer.appendData(data.slice(0, newDataBytes));
	_indexCount += newIndexCount;
}


} // namespace
