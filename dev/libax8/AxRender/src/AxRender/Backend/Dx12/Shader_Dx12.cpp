module AxRender;
import :Shader_Dx12;
import :Renderer_Dx12;

#if AX_RENDERER_DX12

namespace ax {

ShaderPass_Dx12::ShaderPass_Dx12(const CreateDesc& desc)
: Base(desc)
{
//	auto* renderer = Renderer_Dx12::s_instance();
//	auto& dev = renderer->d3dDevice();

	auto loadStage = [&](Stage& stage, ShaderStageFlags stageFlags) {
		if (!desc.info->getFuncName(stageFlags)) return;
		auto filename = Fmt("{}/Dx12/Shader_Dx12-{}-{}.bin", shader()->assetPath(), _name, stageFlags);
		stage.bytecode.openFile(filename);

		// TODO
	};

	_visitStages(loadStage);
	
//	_pipelineLayout.create(dev, layouts);
//---------------	
}

} // namespace

#endif // #if AX_RENDERER_DX12
