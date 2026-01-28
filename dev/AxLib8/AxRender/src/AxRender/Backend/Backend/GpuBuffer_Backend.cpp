module;
module AxRender;
import :GpuBuffer_Backend;
import :RenderSystem_Backend;

namespace ax /*::AxRender*/ {

SPtr<GpuBufferPool_Backend> GpuBufferPool_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(RenderSystem_Backend::s_instance()->newGpuBufferPool(req, desc));
}

GpuBufferPool_Backend::GpuBufferPool_Backend(const CreateDesc& desc): Base(desc) {
	D3D12MA::VIRTUAL_BLOCK_DESC blockDesc = {};

	blockDesc.Size = desc.maxSize;
	HRESULT hr     = D3D12MA::CreateVirtualBlock(&blockDesc, _virtualBlock.ptrForInit());
	_throwIfError(hr);
}

void GpuBufferPool_Backend::_allocateBlock(GpuBuffer* buf) {
	AX_ASSERT(buf->_pool == nullptr);
	
	D3D12MA::VIRTUAL_ALLOCATION_DESC desc = {};
	desc.Size = buf->_size;
	desc.Alignment = _pageSize;
		
	UINT64 offset = 0;
	_virtualBlock->Allocate(&desc, &buf->_virtualAllocation, &offset);

	buf->_pool = this;
	buf->_offset = ax_safe_cast_from(offset);
	
	onAllocateBlock(buf);
}

void GpuBufferPool_Backend::_freeBlock(GpuBuffer* buf) {
	if (buf->_pool != this) throw Error_Undefined();
	if (!buf->_virtualAllocation.AllocHandle) return;
	
	onFreeBlock(buf);
	
	_virtualBlock->FreeAllocation(buf->_virtualAllocation);
	buf->_pool = nullptr;
	buf->_virtualAllocation = {};
	buf->_offset = 0;
}

SPtr<GpuBuffer_Backend> GpuBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(RenderSystem_Backend::s_instance()->newGpuBuffer(req, desc));
}

void GpuBuffer_Backend::copyData(ByteSpan data, Int offset) {
	auto map = mapMemory(Range_StartAndSize(offset, data.size()));
	map->copyValues(data);
}

void GpuBuffer_Backend::copyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) {
	Int  copySize = srcRange.size();
	auto dstRange = Range_StartAndSize(dstOffset, copySize);
	if (! src->inBound(srcRange)) throw Error_IndexOutOfRange();
	if (!this->inBound(dstRange)) throw Error_IndexOutOfRange();
	onCopyFromGpuBuffer(req, src, srcRange, dstOffset);
}

void GpuBuffer_Backend::flush(IntRange range) {
	if (range.size() <= 0) return;

	if (!IntRange(_size).contains(range))
		throw Error_IndexOutOfRange();
	onFlush(range);
}

GpuBuffer_Backend::GpuBuffer_Backend(const CreateDesc& desc): Base(desc) {
	auto* pool = rttiCastCheck<GpuBufferPool_Backend>(desc.pool);
	if (!pool) {
		auto* mgr = RenderObjectManager_Backend::s_instance();
		pool = mgr->_bufferPools.getPool(desc.bufferType);
	}
	
	if (pool && desc.bufferSize) {
		pool->_allocateBlock(this);
	}
}

GpuBuffer_Backend::~GpuBuffer_Backend() {
	if (_pool) {
		rttiCastCheck<GpuBufferPool_Backend>(_pool)->_freeBlock(this);
	}
}

auto GpuStructuredBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) -> SPtr<This> {
	return SPtr_fromUPtr(RenderSystem_Backend::s_instance()->newGpuStructuredBuffer(req, desc));
}

} // namespace