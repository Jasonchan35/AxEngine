module;

export module AxRender:Material_Dx12;
export import :Material_Backend;

#if AX_RENDER_DX12

export import :Dx12DescriptorHeap;
export import :RenderSystem_Backend;
export import :Shader_Dx12;
export import :Texture_Dx12;

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

		void update(Dx12DescriptorHeapChunk& heapChunk) {
			d3dHeap = heapChunk.d3dHeap();
			handle  = heapChunk.currentHandle();
		}
	};

	struct PerFrameData : public NonCopyable {
		HeapStartHandle heapStart_CBV_SRV_UAV;
		HeapStartHandle heapStart_Sampler;
	};

	const PerFrameData& getUpdatedPerFrameData(RenderRequest_Dx12* req) const {
		auto* mut = ax_const_cast(this);
		if (mut->_renderSeqIdGraud.update(req)) {
			return mut->_updatedPerFrameData(req);
		}
		return _perFrameData;
	}
private:
	PerFrameData&    _updatedPerFrameData(RenderRequest_Dx12* req);
	RenderSeqIdGraud _renderSeqIdGraud;
	PerFrameData     _perFrameData;
};

class MaterialPass_Dx12 : public MaterialPass_Backend {
	AX_RTTI_INFO(MaterialPass_Dx12, MaterialPass_Backend)
public:
	using HeapStartHandle = MaterialParamSpace_Dx12::HeapStartHandle;
	
	MaterialPass_Dx12(const CreateDesc& desc) : Base(desc) {}
	
	const ShaderPass_Dx12* shaderPass_dx12() const { return rttiCastCheck<ShaderPass_Dx12>(shaderPass()); }

	const MaterialParamSpace_Dx12* getParamSpace_dx12(BindSpace bs) const {
		return rttiCastCheck<MaterialParamSpace_Dx12>(getParamSpace(bs));
	}

	MaterialParamSpace_Dx12* getOwnParamSpace_dx12(BindSpace bs) {
		return rttiCastCheck<MaterialParamSpace_Dx12>(getOwnParamSpace(bs));
	}	

	virtual bool onBindMaterial(RenderRequest* req, AxVertexShaderDraw& draw, AxVertexShaderDraw_RootConst* rootConst) override;
	virtual bool onBindMaterial(RenderRequest* req, AxMeshShaderDraw  & draw, AxMeshShaderDraw_RootConst  * rootConst) override;
	
	bool _onBindMaterial(RenderRequest_Dx12* req, ByteSpan rootConstData);
	
	virtual void onSetShader() override;
};

class Material_Dx12 : public Material_Backend {
	AX_RTTI_INFO(Material_Dx12, Material_Backend)
public:
	Material_Dx12(const CreateDesc& desc) : Base(desc) {}

	Shader_Dx12*	shader_dx12() { return rttiCastCheck<Shader_Dx12>(Base::shader()); }

	virtual UPtr<MaterialPass_Backend> onNewPass(const MaterialPass_CreateDesc& desc) override {
		return UPtr_new<MaterialPass_Dx12>(AX_NEW, desc);
	}
};

} // namespace

#endif //#if AX_RENDER_DX12
