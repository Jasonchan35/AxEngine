module AxRender;
import :GpuBuffer_Dx12;
import :RenderRequest_Dx12;

#if AX_RENDERER_DX12

namespace  ax {

void GpuBuffer_Dx12::onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) {
	auto dstRange = Range_BeginSize(dstOffset, srcRange.size());
	if (!inBound(dstRange)) throw Error_Undefined();

	static Int copyGpuBufferAlignment = Renderer::s_instance()->copyGpuBufferAlignment();
//	AX_ASSERT(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT == copyGpuBufferAlignment);

	if (!Math::isAlignedTo(srcRange.begin(), copyGpuBufferAlignment)) throw Error_Undefined();
	if (!Math::isAlignedTo(srcRange.end()  , copyGpuBufferAlignment)) throw Error_Undefined();
	if (!Math::isAlignedTo(dstOffset       , copyGpuBufferAlignment)) throw Error_Undefined();
	
	auto* dst_dx12 = this;
	auto* src_dx12 = rttiCastCheck<GpuBuffer_Dx12>(src);
	if (!dst_dx12 || !src_dx12) throw Error_Undefined();

	auto* req_dx12   = rttiCastCheck<RenderRequest_Dx12>(req);
	auto& cmdList_dx = req_dx12->_uploadCmdBuf_dx12._cmdList_dx12;

	auto& srcRes = src_dx12->resource();
	auto& dstRes = dst_dx12->resource();

//	auto srcOldState =
	srcRes.resourceBarrier(cmdList_dx, D3D12_RESOURCE_STATE_COPY_SOURCE);
//	auto dstOldState =
	dstRes.resourceBarrier(cmdList_dx, D3D12_RESOURCE_STATE_COPY_DEST);

	cmdList_dx->CopyBufferRegion(dstRes,
	                             ax_safe_cast_from(dstOffset),
	                             srcRes,
	                             ax_safe_cast_from(srcRange.begin()),
	                             ax_safe_cast_from(srcRange.size()));

//	srcRes.resourceBarrier(cmdList_dx, srcOldState);
//	dstRes.resourceBarrier(cmdList_dx, dstOldState);
}

}

#endif // #if AX_RENDERER_DX12
