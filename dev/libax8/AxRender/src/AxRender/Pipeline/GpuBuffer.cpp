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
	static auto alignment = Renderer::s_instance()->copyGpuBufferAlignment();
	if (!Math::isAlignedTo(desc.bufferSize, alignment))
		throw Error_Undefined("GpuBuffer - bufferSize invalid alignment");
	
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

	static auto copyGpuBufferAlignment = Renderer::s_instance()->copyGpuBufferAlignment();
	auto alignedRange = uploadRange.alignTo(copyGpuBufferAlignment);

	_data.ensureCapacity(alignedRange.end());
	auto dataToCopy = Span(_data.data() + alignedRange.begin(), alignedRange.size());
	
// try inlineUpload
	// if (req->inlineUpload.tryCopyDataToGpuBuffer(_gpuBuffer, dataToCopy, alignedRange.begin())) {
	// 	return _gpuBuffer;
	// }

// use upload buffer
	auto uploadBuf = GpuBuffer_Backend::s_new(AX_ALLOC_REQ,
	                                          Fmt("{}-upload", _name),
	                                          GpuBufferType::StagingToGpu,
	                                          alignedRange.size());

	req->resourcesToKeep.add(uploadBuf.ptr());
	uploadBuf->copyData(_data.slice(uploadRange), uploadRange.begin());

	auto* gpuBuffer = rttiCastCheck<GpuBuffer_Backend>(_gpuBuffer.ptr());
	gpuBuffer->copyFromGpuBuffer(req, uploadBuf, uploadBuf->bufferRange(), alignedRange.begin());

	return _gpuBuffer;
}

} // namespace