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
	_name       = NameId::s_make(desc.name);
	_bufferType = desc.bufferType;
	_bufferSize = desc.bufferSize;
	_virMemDesc = desc.virMemDesc;
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
	_virMemDesc = desc.virMemDesc;
	_data.resize(desc.bufferSize);
}

void DynamicGpuBuffer::destroy() {
	reset();
	_gpuBuffer.unref();
}

void DynamicGpuBuffer::reset() {
	_data.clear();
	_dirtyRange.reset();
}

GpuBuffer* DynamicGpuBuffer::_getUploadedGpuBuffer(RenderRequest* req_) {
	auto* req = rttiCastCheck<RenderRequest_Backend>(req_);
	AX_ASSERT(_bufferType != GpuBufferType::None);
	if (_dirtyRange.size() <= 0 && _gpuBuffer) return _gpuBuffer;

	auto dataSize     = _data.size();
	auto dataCapacity = _data.capacity();
	auto uploadRange  = _dirtyRange;
	_dirtyRange.reset();

	if (!_gpuBuffer || _gpuBuffer->bufferSize() < dataCapacity) {
		GpuBuffer_CreateDesc bufferDesc;
		bufferDesc.bufferType = _bufferType;
		bufferDesc.bufferSize = _virMemDesc.maxSize ? 0 : dataCapacity;
		bufferDesc.virMemDesc = _virMemDesc;

		_gpuBuffer = GpuBuffer::s_new(AX_NEW, bufferDesc);
		uploadRange = IntRange(dataSize); // upload all data for new buffer
	}

	_gpuBuffer->ensureCapacity(req, dataCapacity);
	req->copyDataToGpuBuffer(_gpuBuffer, _data.slice(uploadRange), uploadRange.start());
	return _gpuBuffer;
}

} // namespace