module AxRender;
import :Texture_Dx12;
import :GpuBuffer_Dx12;

#if AX_RENDERER_DX12

namespace ax {

Dx12Descriptor_Sampler Sampler_Dx12::_getUpdatedDescriptor(RenderRequest_Dx12* req) {
	if (!_renderSeqIdGraud.update(req))
		return _descriptor;

	// update
	req->resourcesToKeep.add(this);

	_descriptor = req->_resourceDescriptors->Sampler.setSampler(objectSlot.slotId(), _samplerState);
//	AX_LOG("Sampler#{} debugName=[{}] - setDescriptor({})", objectSlot.slotId(), debugName(), _descriptor.handle);

	return _descriptor;
}

Texture2D_Dx12::Texture2D_Dx12(const CreateDesc& desc): Base(desc) {
}

void Texture2D_Dx12::onImageIO_ReadHandler(ImageIO_ReadHandler& handler) {
	_texResource.create(_info.size.xy(), _info.mipLevels, _info.colorType);
	
	auto dataSize = handler.desc.dataSize;
	_uploadBuffer = GpuBuffer_Backend::s_new(AX_ALLOC_REQ, "Texture2D_VK-upload", GpuBufferType::StagingToGpu, dataSize);
	auto map = _uploadBuffer->mapMemory(IntRange(dataSize));
	handler.readPixelsTo(map.data());
}

Dx12Descriptor_Texture2D Texture2D_Dx12::_getUpdatedDescriptor(RenderRequest_Dx12* req) {
	if (!_renderSeqIdGraud.update(req))
		return _descriptor;

	// update
	req->resourcesToKeep.add(this);

	if (auto* uploadBuf = rttiCast<GpuBuffer_Dx12>(_uploadBuffer.ptr())) {
		auto& desc = _texResource.desc();
		
		auto& cmdList = req->_uploadCmdList_dx12;
		req->resourcesToKeep.add(uploadBuf);

		D3D12_TEXTURE_COPY_LOCATION srcLoc = {};
		srcLoc.pResource = uploadBuf->resource().d3dResource();
		srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

		auto& fp = srcLoc.PlacedFootprint;
		fp.Offset             = 0;
		fp.Footprint.Width    = Dx12Util::castUINT(desc.Width );
		fp.Footprint.Height   = Dx12Util::castUINT(desc.Height);
		fp.Footprint.Depth    = desc.DepthOrArraySize;
		fp.Footprint.Format   = desc.Format;
		fp.Footprint.RowPitch = Dx12Util::castUINT(_info.strideInBytes);

		// TODO - 
		// if (fp.Footprint.RowPitch % D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) {
		// 	throw Error_Undefined(Fmt("Error: Texture RowPitch must be align to {}",
		// 								D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
		// }
		
		D3D12_TEXTURE_COPY_LOCATION dstLoc = {};
		dstLoc.pResource = _texResource.d3dResource();
		dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLoc.SubresourceIndex = 0;

		_texResource.resourceBarrier(cmdList, D3D12_RESOURCE_STATE_COPY_DEST);

		D3D12_BOX box;
		box.left   = 0;
		box.right  = Dx12Util::castUINT(desc.Width);
		box.top    = 0;
		box.bottom = Dx12Util::castUINT(desc.Height);
		box.front  = 0;
		box.back   = 1;
		
		cmdList->CopyTextureRegion(&dstLoc, box.left, box.top, box.front, &srcLoc, &box);
		_texResource.resourceBarrier(cmdList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		_descriptor = req->_resourceDescriptors->Texture2D.setTexture2D(objectSlot.slotId(), _texResource);
//		AX_LOG("Texture2D#{} debugName=[{}] - setDescriptor({})", objectSlot.slotId(), debugName(), _descriptor.handle);
		
		_uploadBuffer = nullptr;
	}

	return _descriptor;
}

} // namespace

#endif //AX_RENDERER_DX12
