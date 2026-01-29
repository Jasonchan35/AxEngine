module AxRender;
import :RenderSystem_Backend;
import :RenderRequest_Backend;
import :RenderContext_Backend;
import :GpuBuffer_Backend;

namespace ax /*::AxRender*/ {

SPtr<GpuBuffer> GpuBuffer::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return GpuBuffer_Backend::s_new(req, desc);
}

GpuBuffer::GpuBuffer(const CreateDesc& desc) {
	_name = NameId::s_make(desc.name);
	_type = desc.bufferType;
	_size = desc.bufferSize;
	_pool = desc.pool;
}

GpuBufferPool::GpuBufferPool(const CreateDesc& desc)
: _bufferType(desc.bufferType)
, _maxSize(desc.maxSize)
, _pageSize(desc.pageSize) 
{
	setName(desc.name);
}

SPtr<StructuredGpuBuffer> StructuredGpuBuffer::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return StructuredGpuBuffer_Backend::s_new(req, desc);
}

StructuredGpuBuffer::StructuredGpuBuffer(const CreateDesc& desc) {
	_stride = desc.stride;
	
	DynamicGpuBuffer_CreateDesc bufDesc;
	bufDesc.name       = desc.name;
	bufDesc.bufferType = GpuBufferType::Structured;
	bufDesc.bufferSize = 0;
	bufDesc.pool       = desc.pool;

	_gpuBuffer.create(bufDesc);
}

void DynamicGpuBuffer::create(const CreateDesc& desc) {
	reset();
	_name       = desc.name;
	_bufferType = desc.bufferType;
	_pool       = desc.pool;
	_cpuBuffer.resize(desc.bufferSize);
}

void DynamicGpuBuffer::destroy() {
	reset();
	_gpuBuffer.unref();
}

void DynamicGpuBuffer::reset() {
	_cpuBuffer.clear();
	_dirtyRange.reset();
}

GpuBuffer* DynamicGpuBuffer::_getUploadedGpuBuffer(RenderRequest* req_) {
	auto* req = rttiCastCheck<RenderRequest_Backend>(req_);
	AX_ASSERT(_bufferType != GpuBufferType::None);
	if (_dirtyRange.size() <= 0 && _gpuBuffer) return _gpuBuffer;

	auto dataSize     = _cpuBuffer.size();
	auto dataCapacity = _cpuBuffer.capacity();
	auto uploadRange  = _dirtyRange;
	_dirtyRange.reset();

	if (!_gpuBuffer || _gpuBuffer->size() < dataCapacity) {
		GpuBuffer_CreateDesc bufferDesc;
		bufferDesc.bufferType = _bufferType;
		bufferDesc.bufferSize = dataCapacity;
		bufferDesc.pool       = _pool;

		_gpuBuffer = GpuBuffer::s_new(AX_NEW, bufferDesc);
		uploadRange = IntRange(dataSize); // upload all data for new buffer
	}

	req->copyDataToGpuBuffer(_gpuBuffer, _cpuBuffer.slice(uploadRange), uploadRange.start());
	return _gpuBuffer;
}

} // namespace