module AxRender;
import :GpuBuffer_Dx12;
import :RenderRequest_Dx12;

#if AX_RENDERER_DX12

namespace  ax {

void GpuBuffer_Dx12::onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) {
	auto* dst_dx12 = this;
	auto* src_dx12 = rttiCastCheck<GpuBuffer_Dx12>(src);
	if (!dst_dx12 || !src_dx12) throw Error_Undefined();

	auto* req_dx12   = rttiCastCheck<RenderRequest_Dx12>(req);
	auto& cmdList_dx = req_dx12->_uploadCmdBuf_dx12._cmdList_dx12;

	src_dx12->resource().resourceBarrier(cmdList_dx, D3D12_RESOURCE_STATE_COPY_SOURCE);
	dst_dx12->resource().resourceBarrier(cmdList_dx, D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList_dx->CopyResource(dst_dx12->d3dResource(), src_dx12->d3dResource());
}

}

#endif // #if AX_RENDERER_DX12
