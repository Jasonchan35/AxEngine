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
}

GpuBufferPool::GpuBufferPool(const CreateDesc& desc)
: _bufferType(desc.bufferType)
, _maxSize(desc.maxSize)
, _pageSize(desc.pageSize) 
{
	setName(desc.name);

}

SPtr<GpuStructuredBuffer> GpuStructuredBuffer::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return GpuStructuredBuffer_Backend::s_new(req, desc);
}

GpuStructuredBuffer::GpuStructuredBuffer(const CreateDesc& desc) {
	_stride   = desc.stride;
	_capacity = desc.capacity;
	
	DynamicGpuBuffer_CreateDesc createDesc;
	createDesc.bufferType = GpuBufferType::Structured;
	createDesc.name = desc.name;
	_buffer.create(createDesc);
	_buffer.ensureDataCapacity(desc.capacity * desc.stride);
}

void DynamicGpuBuffer::create(const CreateDesc& desc) {
	reset();
	_name       = desc.name;
	_bufferType = desc.bufferType;
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

		_gpuBuffer = GpuBuffer::s_new(AX_NEW, bufferDesc);
		uploadRange = IntRange(dataSize); // upload all data for new buffer
	}

	req->copyDataToGpuBuffer(_gpuBuffer, _cpuBuffer.slice(uploadRange), uploadRange.start());
	return _gpuBuffer;
}

} // namespace