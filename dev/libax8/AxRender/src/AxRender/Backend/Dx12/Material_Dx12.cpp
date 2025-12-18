module AxRender;
import :Material_Dx12;
import :Texture_Dx12;
import :GpuBuffer_Dx12;

#if AX_RENDERER_DX12

namespace ax {

void MaterialParamSpace_Dx12::_onDrawcall(RenderRequest_Dx12* req, bool isCompute) {
	auto* shdSpace = rttiCastCheck<ShaderParamSpace_Dx12>(_shaderParamSpace.ptr());
	
	auto& cmdList = req->_graphCmdBuf_dx12;
	
	Int cbIndex = 0;
	for (auto& cb : _constBuffers) {
		auto* gpuBuf = rttiCastCheck<GpuBuffer_Dx12>(cb.getUploadedGpuBuffer(req));
		if (!gpuBuf) throw Error_Undefined();

		gpuBuf->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		if (isCompute) {
			cmdList->SetComputeRootConstantBufferView(ax_safe_cast_from(cbIndex), gpuBuf->gpuAddress());
		} else {
			cmdList->SetGraphicsRootConstantBufferView(ax_safe_cast_from(cbIndex), gpuBuf->gpuAddress());
		}
		cbIndex++;
	}

	cmdList->SetGraphicsRootDescriptorTable(ax_safe_cast_from(shdSpace->_descTableIndexInRoot),
	                                        _texDescHeap.handleStart().gpu);
	
	cmdList->SetGraphicsRootDescriptorTable(ax_safe_cast_from(shdSpace->_samplerDescTableIndexInRoot),
	                                        _samplerDescHeap.handleStart().gpu);
}

bool MaterialPass_Dx12::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	if (!req) { AX_ASSERT(false); return false; }

	auto* shdPass = shaderPass_dx12();
	if (!shdPass) { AX_ASSERT(false); return false; }

	if (!shdPass->_bindPipeline(req, cmd)) return false;

	auto* renderer = Renderer_Backend::s_instance();

	for (auto& paramSpace_ : _materialParamSpaces) {
		if (!paramSpace_) continue;

		auto* paramSpace = rttiCastCheck<MaterialParamSpace_Dx12>(paramSpace_.ptr());
		if (!paramSpace) { AX_ASSERT(false); return false; }

		auto spaceType = ax_enum_int(paramSpace->paramSpaceType());
		if (spaceType >= ax_enum_int(SpaceType::_COUNT)) {
			AX_ASSERT(false);
			return false;
		}

		paramSpace->_onDrawcall(req, shdPass->isCompute());
	}

	auto* commonMaterial = renderer->commonMaterial();
	if (!commonMaterial) { AX_ASSERT(false); return false; }

	return true;
}

} // namespace

#endif //AX_RENDERER_DX12
