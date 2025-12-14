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
	for (auto& slot : _slots) {
		slot.dirtyRange.reset();
	}
}

GpuBuffer* DynamicGpuBuffer::getUploadedGpuBuffer(RenderRequest* req_) {
	AX_ASSERT(_bufferType != GpuBufferType::None);

	if (_dirtyRange.size() <= 0 && _gpuBuffer) return _gpuBuffer;

	auto  dataSize = _data.size();
	auto uploadRange = _dirtyRange;

	_dirtyRange.reset();

	auto* req = rttiCastCheck<RenderRequest_Backend>(req_);
	if (!req) { AX_ASSERT(false); return nullptr; }

	if (!_gpuBuffer || _gpuBuffer->bufferSize() < dataSize) {
		uploadRange = IntRange(dataSize); // upload all for new buffer
		_gpuBuffer = GpuBuffer::s_new(AX_ALLOC_REQ, _name, _bufferType, dataSize);
	}

// inlineUpload
	if (req->inlineUpload.copyDataToGpuBuffer(
			rttiCastCheck<GpuBuffer_Backend>(_gpuBuffer.ptr()),
			_data.slice(uploadRange),
			uploadRange.begin()))
	{
		_dirtyRange.reset();
		return _gpuBuffer;
	}

//----- use slot ---
	{ // next slot
		auto renderSeqId = req->renderSeqId();
		if (_lastUpdateRenderSeqId == renderSeqId) {
			AX_ASSERT(false); // don't upload twice in same frame

		} else {
			_lastUpdateRenderSeqId = req->renderSeqId();

			Int maxSlots = req->renderer()->renderRequestCount();
			_currentSlotIndex = (_currentSlotIndex + 1) % maxSlots;

			if (_slots.size() <= _currentSlotIndex)
				_slots.resize(_currentSlotIndex + 1);
		}
	}

	auto& slot = _slots[_currentSlotIndex];
	uploadRange |= slot.dirtyRange;

	auto& uploadBuf = slot.uploadBuffer;

	if (!uploadBuf || uploadBuf->bufferSize() < dataSize) {
		uploadRange = IntRange(dataSize); // upload all for new buffer
		uploadBuf = GpuBuffer::s_new(AX_ALLOC_REQ, Fmt("{}-upload", _name), GpuBufferType::StagingToGpu, dataSize);
	}

	auto* uploadBuf_Backend = rttiCastCheck<GpuBuffer_Backend>(uploadBuf.ptr());

	req->resourcesToKeep.add(uploadBuf_Backend);

	uploadBuf_Backend->copyData(_data.slice(uploadRange), uploadRange.begin());
	rttiCastCheck<GpuBuffer_Backend>(_gpuBuffer.ptr())->copyFromGpuBuffer(req, uploadBuf, uploadRange, uploadRange.begin());

	slot.dirtyRange.reset();
	return _gpuBuffer;
}


RenderColorBuffer::RenderColorBuffer(const CreateDesc& desc) {
	_name			= desc.name;
	_colorType		= desc.colorType;
	_size			= desc.size;
	_backBufferRef	= desc.backBufferRef;
}

void RenderColorBuffer_CreateDesc::setBackBuffer(
	RenderContext* renderContext_, 
	Int index_, 
	ColorType colorType_,
	const Vec2i& size_
) {
	colorType	= colorType_;
	size		= size_;

	backBufferRef.renderContext	= renderContext_;
	backBufferRef.index			= index_;
}

RenderDepthBuffer::RenderDepthBuffer(const CreateDesc& desc) 
: Base(RenderDataType::DepthTexture)
{
	_type	   = RenderDataType::DepthTexture;
	_depthType = desc.depthType;
	_frameSize = desc.frameSize;
}

} // namespace