module AxRender;
import :GpuBuffer_Dx12;
import :RenderRequest_Dx12;

#if AX_RENDER_DX12

namespace  ax {

GpuBufferPool_Dx12::GpuBufferPool_Dx12(const CreateDesc& desc): Base(desc) {
	_resource_dx12.create(desc.bufferType, desc.maxSize, true);
	_resource_dx12.setName(desc.name);
	_blockAlignment = desc.blockAlignment ? desc.blockAlignment : GpuBuffer_Dx12::s_getMinAlignement(desc.bufferType);
	_pagePool.create(desc);
}

void GpuBufferPool_Dx12::onGpuUpdatePages(RenderRequest_Backend* req_) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	
	auto& cmdList = req->uploadCmdList_dx12();
	_resource_dx12.resourceBarrier(cmdList, Dx12Resource_GpuBuffer::s_defaultResourceState(_resource_dx12.bufferType()));
	
	auto* cmdQueue = req->renderContext_dx12()->graphCmdQueue().queue();
	
	for (auto& pageIndex : _pagePool._pendingCommitPages) {
		auto& page = _pagePool._pages[pageIndex];
		Int resourceOffset = ax_safe_cast_from(pageIndex * _pageSize);
		
		D3D12_HEAP_DESC heapDesc = {};
		heapDesc.SizeInBytes = _pageSize;
		heapDesc.Alignment   = 0;
		heapDesc.Properties  = {};
		heapDesc.Properties.Type = _resource_dx12.allocDesc().HeapType;
		auto hr = req->_d3dDevice->CreateHeap(&heapDesc, IID_PPV_ARGS(page._d3dHeap.ptrForInit()));
		Dx12Util::throwIfError(hr);

		D3D12_TILED_RESOURCE_COORDINATE coordinate = {};
		coordinate.X = ax_safe_cast_from(resourceOffset / kTileSizeInBytes);
			
		D3D12_TILE_REGION_SIZE regionSize = {};
		regionSize.Width    = ax_safe_cast_from(_pageSize / kTileSizeInBytes);
		regionSize.Height   = 1;
		regionSize.Depth    = 1;
		regionSize.NumTiles = regionSize.Width * regionSize.Height * regionSize.Depth;
			
		D3D12_TILE_RANGE_FLAGS tileRangeFlags = D3D12_TILE_RANGE_FLAG_NONE;
			
		UINT heapRangeStartOffsets = 0;
		UINT rangeTileCounts = regionSize.NumTiles;
			
		// AX_LOG("--- Dx12 UpdateTileMappings {} {}", _resource_dx12.bufferType(), resourceOffset);
		cmdQueue->UpdateTileMappings(_resource_dx12.d3dResource(), 
			1, &coordinate, &regionSize, page._d3dHeap, 
			1, &tileRangeFlags, &heapRangeStartOffsets, &rangeTileCounts,
			D3D12_TILE_MAPPING_FLAG_NONE);
	}
	
	_pagePool._pendingCommitPages.clear();
}

GpuBuffer_Dx12::GpuBuffer_Dx12(const CreateDesc& desc): Base(desc) {
	if (_pool) {
		auto* pool = rttiCastCheck<GpuBufferPool_Dx12>(_pool);
		pool->_allocateBlock(this);
		_d3dResource.ref(pool->_resource_dx12.d3dResource());
		_gpuAddress  = pool->_resource_dx12.gpuAddress();
		
	} else {
		_resourceWithoutPool.create(_type, _size, false);
		_resourceWithoutPool.setName(desc.name);
		_size = _resourceWithoutPool.bufferSize(); // size may change caused by alignment
		_d3dResource.ref(_resourceWithoutPool.d3dResource());
		_gpuAddress = _resourceWithoutPool.gpuAddress();
	}
}

void GpuBuffer_Dx12::onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) {
	auto dstRange = Range_StartAndSize(dstOffset, srcRange.size());
	if (!inBound(dstRange)) throw Error_Undefined();
	
	auto* dst_dx12 = this;
	auto* src_dx12 = rttiCastCheck<GpuBuffer_Dx12>(src);
	if (!dst_dx12 || !src_dx12) throw Error_Undefined();

	auto* req_dx12   = rttiCastCheck<RenderRequest_Dx12>(req);
	auto& cmdList_dx = req_dx12->_uploadCmdList_dx12._cmdList_dx12;
//	auto& cmdList_dx = req_dx12->_graphCmdList_dx12._cmdList_dx12;

	auto* srcRes = src_dx12->resource_dx12();
	auto* dstRes = dst_dx12->resource_dx12();

//	auto srcOldState =
	srcRes->resourceBarrier(cmdList_dx, D3D12_RESOURCE_STATE_COPY_SOURCE);
//	auto dstOldState =
	dstRes->resourceBarrier(cmdList_dx, D3D12_RESOURCE_STATE_COPY_DEST);

	cmdList_dx->CopyBufferRegion(dstRes->d3dResource(),
	                             ax_safe_cast_from(dstOffset + dst_dx12->_bufferOffsetInBytes),
	                             srcRes->d3dResource(),
	                             ax_safe_cast_from(srcRange.start() + src_dx12->_bufferOffsetInBytes),
	                             ax_safe_cast_from(srcRange.size()));

//	srcRes->resourceBarrier(cmdList_dx, srcOldState);
//	dstRes->resourceBarrier(cmdList_dx, dstOldState);
}

}

#endif // #if AX_RENDER_DX12
