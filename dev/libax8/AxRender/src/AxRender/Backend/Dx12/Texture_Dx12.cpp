module AxRender;
import :Texture_Dx12;
import :GpuBuffer_Dx12;

#if AX_RENDERER_DX12

namespace ax {

Texture2D_Dx12::Texture2D_Dx12(const CreateDesc& desc): Base(desc) {
}

void Texture2D_Dx12::onImageIO_ReadHandler(ImageIO_ReadHandler& handler) {
	_texResource.create(_info.size.xy(), _info.mipLevels, _info.colorType);
	
	auto dataSize = handler.desc.dataSize;
	_uploadBuffer = GpuBuffer_Backend::s_new(AX_ALLOC_REQ, "Texture2D_VK-upload", GpuBufferType::StagingToGpu, dataSize);
	auto map = _uploadBuffer->mapMemory(IntRange(dataSize));
	handler.readPixelsTo(map.data());
}

Dx12Resource_Texture2D& Texture2D_Dx12::_bindImage(RenderRequest_Dx12* req) {
	req->resourcesToKeep.add(this);

	if (auto* uploadBuf = rttiCast<GpuBuffer_Dx12>(_uploadBuffer.ptr())) {
		auto& desc = _texResource.desc();
		
		auto& cmdBuf = req->_uploadCmdBuf_dx12;
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

		_texResource.resourceBarrier(cmdBuf, D3D12_RESOURCE_STATE_COPY_DEST);

		D3D12_BOX box;
		box.left   = 0;
		box.right  = Dx12Util::castUINT(desc.Width);
		box.top    = 0;
		box.bottom = Dx12Util::castUINT(desc.Height);
		box.front  = 0;
		box.back   = 1;
		
		cmdBuf->CopyTextureRegion(&dstLoc, box.left, box.top, box.front, &srcLoc, &box);
		_texResource.resourceBarrier(cmdBuf, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		_uploadBuffer = nullptr;
	}

	return _texResource;
}

} // namespace

#endif //AX_RENDERER_DX12
