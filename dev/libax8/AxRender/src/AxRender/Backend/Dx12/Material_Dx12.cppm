module;

export module AxRender:Material_Dx12;
import :Dx12DescripterHeap;

#if AX_RENDERER_DX12

import :RenderSystem_Backend;
import :Material_Backend;
import :Shader_Dx12;

namespace ax {

class MaterialParamSpace_Dx12 : public MaterialParamSpace_Backend {
	AX_RTTI_INFO(MaterialParamSpace_Dx12, MaterialParamSpace_Backend)
public:
	MaterialParamSpace_Dx12(const CreateDesc& desc) : Base(desc){}

	const ShaderParamSpace_Dx12* shaderParamSpace_dx12() const {
		return rttiCastCheck<ShaderParamSpace_Dx12>(_shaderParamSpace.ptr());
	}

	struct HeapStartHandle : public NonCopyable {
		ID3D12DescriptorHeap* d3dHeap = nullptr;
		Dx12DescriptorHandle  handle;
		Int bindCount = 0;

		void update(Dx12DescripterHeapBlock& block) {
			d3dHeap = block.d3dHeap();
			handle = block.startHandle();
			bindCount = 0;
		}
	};

	struct PerFrameData : public NonCopyable {
		HeapStartHandle _CBV_SRV_UAV;
		HeapStartHandle _sampler;
	};

	const PerFrameData& getUpdatedPerFrameData(RenderRequest_Dx12*                    req,
	                                           Dx12DescripterHeap_CBV_SRV_UAV::Block& cbvHeapBlock,
	                                           Dx12DescripterHeap_Sampler::Block&     samplerHeapBlock
	) const {
		if (_lastRenderSeqId == req->renderSeqId()
		 && cbvHeapBlock.d3dHeap() == _perFrameData._CBV_SRV_UAV.d3dHeap
		 && samplerHeapBlock.d3dHeap() == _perFrameData._sampler.d3dHeap)
		{
			// using same heap, so no update needed
			return _perFrameData;
		}
		return ax_const_cast(this)->_updatedPerFrameData(req, cbvHeapBlock, samplerHeapBlock);
	}
private:
	PerFrameData& _updatedPerFrameData(RenderRequest_Dx12*                    req,
										 Dx12DescripterHeap_CBV_SRV_UAV::Block& cbvHeapBlock,
										 Dx12DescripterHeap_Sampler::Block&     samplerHeapBlock);
	
	RenderSeqId  _lastRenderSeqId = 0;
	PerFrameData _perFrameData;
};

class MaterialPass_Dx12 : public MaterialPass_Backend {
	AX_RTTI_INFO(MaterialPass_Dx12, MaterialPass_Backend)
public:
	MaterialPass_Dx12(const CreateDesc& desc) : Base(desc) {}
	
	const ShaderPass_Dx12* shaderPass_dx12() const { return rttiCastCheck<ShaderPass_Dx12>(shaderPass()); }

	const MaterialParamSpace_Dx12* getParamSpace_dx12(BindSpace bs) const {
		return rttiCastCheck<MaterialParamSpace_Dx12>(getParamSpace(bs));
	}

	MaterialParamSpace_Dx12* getOwnParamSpace_dx12(BindSpace bs) {
		return rttiCastCheck<MaterialParamSpace_Dx12>(getOwnParamSpace(bs));
	}	
	
	virtual bool onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) override;
	virtual void onSetShader() override;
};

class Material_Dx12 : public Material_Backend {
	AX_RTTI_INFO(Material_Dx12, Material_Backend)
public:
	Material_Dx12(const CreateDesc& desc) : Base(desc) {}

	Shader_Dx12*	shader_dx12() { return rttiCastCheck<Shader_Dx12>(Base::shader()); }

	virtual UPtr<MaterialPass_Backend> onNewPass(const MaterialPass_CreateDesc& desc) override {
		return UPtr_new<MaterialPass_Dx12>(AX_ALLOC_REQ, desc);
	}
};

} // namespace

#endif //#if AX_RENDERER_DX12
