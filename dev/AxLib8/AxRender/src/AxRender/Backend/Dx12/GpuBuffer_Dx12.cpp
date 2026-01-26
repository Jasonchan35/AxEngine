module AxRender;
import :GpuBuffer_Dx12;
import :RenderRequest_Dx12;

#if AX_RENDERER_DX12

namespace  ax {

GpuBuffer_Dx12::GpuBuffer_Dx12(const CreateDesc& desc): Base(desc) {
	_p.create(desc.bufferType, desc.bufferSize, desc.virtualMemMaxSize, desc.virtualMemPageSize);
#if AX_RENDER_DEBUG_NAME
	_p.setDebugName(desc.name);
#endif
}

void GpuBuffer_Dx12::onSetCapacity(RenderRequest* req_, Int newCapacity) {
	if (_virtualMemMaxSize <= 0) throw Error_Undefined();
	
	newCapacity = Math::alignTo(newCapacity, _virtualMemPageSize);
	
	if (!_virtualMemData) {
		_virtualMemData = UPtr_new<VirtualMemData>(AX_NEW);
		auto pageCount = Math::alignTo(_virtualMemMaxSize, _virtualMemPageSize) / _virtualMemPageSize;
		_virtualMemData->_memPages.resize(pageCount);
	}
	
	auto oldPageCount = Math::alignTo(_bufferSize, _virtualMemPageSize) / _virtualMemPageSize;
	auto newPageCount = Math::alignTo(newCapacity, _virtualMemPageSize) / _virtualMemPageSize;
	
	auto* dev = RenderSystem_Dx12::s_d3dDevice();
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	auto* cmdQueue = req->renderContext_dx12()->graphCmdQueue().queue();
	
	auto addedPageCount = newPageCount - oldPageCount;
	for (Int i = 0; i < addedPageCount; ++i) {
		Int pageIndex = oldPageCount + i;
		auto& page = _virtualMemData->_memPages[pageIndex];
		if (page._d3dHeap) continue;
		
		auto& allocDesc = _p.allocDesc();
		
		D3D12_HEAP_DESC heapDesc = {};
		heapDesc.SizeInBytes = _virtualMemPageSize;
		heapDesc.Alignment   = _virtualMemPageSize;
		heapDesc.Properties  = {};
		heapDesc.Properties.Type = allocDesc.HeapType;
		auto hr = dev->CreateHeap(&heapDesc, IID_PPV_ARGS(page._d3dHeap.ptrForInit()));
		Dx12Util::throwIfError(hr);

		D3D12_TILED_RESOURCE_COORDINATE coordinate = {};
		coordinate.X = ax_safe_cast_from(pageIndex * _virtualMemPageSize);
		
		D3D12_TILE_REGION_SIZE regionSize = {};
		regionSize.NumTiles = 1;
		regionSize.Width = ax_safe_cast_from(_virtualMemPageSize);
		
		D3D12_TILE_RANGE_FLAGS tileRangeFlags = D3D12_TILE_RANGE_FLAG_NONE;
		
		UINT heapRangeStartOffsets = 0;
		UINT rangeTileCounts = 1;
		
		cmdQueue->UpdateTileMappings(_p, 
			1, &coordinate, &regionSize, page._d3dHeap, 
			1, &tileRangeFlags, &heapRangeStartOffsets, &rangeTileCounts,
			D3D12_TILE_MAPPING_FLAG_NONE);
	}

	_bufferSize = newCapacity;
}

void GpuBuffer_Dx12::onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) {
	auto dstRange = Range_StartAndSize(dstOffset, srcRange.size());
	if (!inBound(dstRange)) throw Error_Undefined();
	
	auto* dst_dx12 = this;
	auto* src_dx12 = rttiCastCheck<GpuBuffer_Dx12>(src);
	if (!dst_dx12 || !src_dx12) throw Error_Undefined();

	auto* req_dx12   = rttiCastCheck<RenderRequest_Dx12>(req);
	auto& cmdList_dx = req_dx12->_uploadCmdList_dx12._cmdList_dx12;

	auto& srcRes = src_dx12->resource();
	auto& dstRes = dst_dx12->resource();

//	auto srcOldState =
	srcRes.resourceBarrier(cmdList_dx, D3D12_RESOURCE_STATE_COPY_SOURCE);
//	auto dstOldState =
	dstRes.resourceBarrier(cmdList_dx, D3D12_RESOURCE_STATE_COPY_DEST);

	cmdList_dx->CopyBufferRegion(dstRes,
	                             ax_safe_cast_from(dstOffset),
	                             srcRes,
	                             ax_safe_cast_from(srcRange.start()),
	                             ax_safe_cast_from(srcRange.size()));

//	srcRes.resourceBarrier(cmdList_dx, srcOldState);
//	dstRes.resourceBarrier(cmdList_dx, dstOldState);
}

}

#endif // #if AX_RENDERER_DX12
