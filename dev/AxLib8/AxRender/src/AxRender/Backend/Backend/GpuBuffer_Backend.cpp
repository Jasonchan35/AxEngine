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

GpuBufferPool::Statistics GpuBufferPool_Backend::getStatistics() const {
	Statistics o;
	D3D12MA::Statistics src;
	_virtualBlock.ptr()->GetStatistics(&src);
	
	o.allocationCount = ax_safe_cast_from(src.AllocationCount);
	o.sizeInBytes     = ax_safe_cast_from(src.AllocationBytes);
	return o;
}

void GpuBufferPool_Backend::_allocateBlock(GpuBuffer* buf) {
	D3D12MA::VIRTUAL_ALLOCATION_DESC desc = {};
	if (_blockAlignment <= 0) {
		throw Error_Undefined("GpuBufferPool _blockAlignment <= 0");
	}
	
	if (_bufferType != buf->_type) {
		throw Error_Undefined("GpuBufferPool type != GpuBuffer type");
	}
	
	buf->_size = Math::alignTo(buf->_size, _blockAlignment);
	
	desc.Alignment = _blockAlignment;
	desc.Size      = ax_safe_cast_from(buf->_size);
		
	UINT64 offset = 0;
	_virtualBlock->Allocate(&desc, &buf->_virtualAllocation, &offset);

	buf->_bufferOffsetInBytes = ax_safe_cast_from(offset);
	onAllocateBlock(buf);
}

void GpuBufferPool_Backend::_freeBlock(GpuBuffer* buf) {
	if (buf->_pool != this) throw Error_Undefined();
	if (!buf->_virtualAllocation.AllocHandle) return;
	
	onFreeBlock(buf);
	
	_virtualBlock->FreeAllocation(buf->_virtualAllocation);
	buf->_pool = nullptr;
	buf->_virtualAllocation = {};
	buf->_bufferOffsetInBytes = 0;
}

SPtr<GpuBuffer_Backend> GpuBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(RenderSystem_Backend::s_instance()->newGpuBuffer(req, desc));
}

void GpuBuffer_Backend::copyData(ByteSpan data, Int offset) {
	auto range = IntRange_StartAndSize(offset, data.size());
	auto map = mapMemory(range);
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
}

GpuBuffer_Backend::~GpuBuffer_Backend() {
	if (_pool) {
		rttiCastCheck<GpuBufferPool_Backend>(_pool)->_freeBlock(this);
	}
}

auto StructuredGpuBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) -> SPtr<This> {
	return SPtr_fromUPtr(RenderSystem_Backend::s_instance()->newStructuredGpuBuffer(req, desc));
}

} // namespace