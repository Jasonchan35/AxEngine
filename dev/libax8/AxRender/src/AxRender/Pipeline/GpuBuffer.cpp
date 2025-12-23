module AxRender;
import :Renderer_Backend;
import :RenderRequest_Backend;
import :RenderContext_Backend;
import :GpuBuffer_Backend;

namespace ax /*::AxRender*/ {

SPtr<GpuBuffer> GpuBuffer::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return GpuBuffer_Backend::s_new(req, desc);
}

GpuBuffer::GpuBuffer(const CreateDesc& desc) {
	_bufferType = desc.bufferType;
	_bufferSize = desc.bufferSize;
}

StorageBuffer::StorageBuffer(const CreateDesc& desc) {
	GpuBuffer_CreateDesc gpuBufDesc;
	gpuBufDesc.bufferType = GpuBufferType::Storage;
	gpuBufDesc.bufferSize = desc.bufferSize;

	_gpuBuffer = GpuBuffer::s_new(AX_ALLOC_REQ, gpuBufDesc);
}

void DynamicGpuBuffer::create(const CreateDesc& desc) {
	reset();
	_name = desc.name;
	_bufferType = desc.bufferType;
	_data.resize(desc.bufferSize);
}

void DynamicGpuBuffer::reset() {
	_data.clear();
	_dirtyRange.reset();
}

GpuBuffer* DynamicGpuBuffer::getUploadedGpuBuffer(RenderRequest* req_) {
	auto* req = rttiCastCheck<RenderRequest_Backend>(req_);
	AX_ASSERT(_bufferType != GpuBufferType::None);
	if (_dirtyRange.size() <= 0 && _gpuBuffer) return _gpuBuffer;

	auto dataSize    = _data.size();
	auto uploadRange = _dirtyRange;
	_dirtyRange.reset();
	
	if (!_gpuBuffer || _gpuBuffer->bufferSize() < dataSize) {
		_gpuBuffer = GpuBuffer::s_new(AX_ALLOC_REQ, _name, _bufferType, dataSize);
		uploadRange = IntRange(dataSize); // upload all for new buffer
	}

	req->copyDataToGpuBuffer(_gpuBuffer, _data.slice(uploadRange), uploadRange.start());
	return _gpuBuffer;
}

} // namespace