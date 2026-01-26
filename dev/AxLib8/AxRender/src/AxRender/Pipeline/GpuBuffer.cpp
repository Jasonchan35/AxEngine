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
	_name               = NameId::s_make(desc.name);
	_bufferType         = desc.bufferType;
	_bufferSize         = desc.bufferSize;
	_virtualMemMaxSize  = desc.virtualMemMaxSize;
	_virtualMemPageSize = desc.virtualMemPageSize;
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
	_name               = desc.name;
	_bufferType         = desc.bufferType;
	_virtualMemMaxSize  = desc.virtualMemMaxSize;
	_virtualMemPageSize = desc.virtualMemPageSize;
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
		if (_gpuBuffer && _virtualMemMaxSize) {
			_gpuBuffer->ensureCapacity(req, dataCapacity);
			
		} else {
			GpuVirtualAllocator_CreateDesc allocatorDesc;
			allocatorDesc.bufferType = _bufferType;
			auto gpuAllocator = GpuVirtualAllocator_Backend::s_new(AX_NEW, allocatorDesc);
			
			GpuBuffer_CreateDesc bufferDesc;
			bufferDesc.bufferType         = _bufferType;
			bufferDesc.bufferSize         = dataCapacity;
			bufferDesc.virtualMemMaxSize  = _virtualMemMaxSize;
			bufferDesc.virtualMemPageSize = _virtualMemPageSize;
			
			_gpuBuffer = GpuBuffer::s_new(AX_NEW, bufferDesc);
			uploadRange = IntRange(dataSize); // upload all data for new buffer
		}
	}

	req->copyDataToGpuBuffer(_gpuBuffer, _data.slice(uploadRange), uploadRange.start());
	return _gpuBuffer;
}

} // namespace