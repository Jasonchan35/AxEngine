module;

export module AxRender:Texture_Dx12;
#if AX_RENDERER_DX12

export import :Dx12Resource;
export import :Texture_Backend;
export import :RenderSystem_Backend;
export import :GpuBuffer_Backend;
export import :RenderRequest_Dx12;

namespace ax {

class Sampler_Dx12 : public Sampler_Backend {
	AX_RTTI_INFO(Sampler_Dx12, Sampler_Backend)
public:
	Sampler_Dx12(const CreateDesc& desc) : Base(desc) {}
};

class Texture2D_Dx12 : public Texture2D_Backend {
	AX_RTTI_INFO(Texture2D_Dx12, Texture2D_Backend)
public:
	Texture2D_Dx12(const CreateDesc& desc);

	virtual void onImageIO_ReadHandler(ImageIO_ReadHandler& handler) override;

	Dx12Descriptor_Texture2D _bindImage(RenderRequest_Dx12* req);

private:	
	Dx12Resource_Texture2D		_texResource;
	SPtr<GpuBuffer_Backend>		_uploadBuffer;
	Dx12Descriptor_Texture2D	_descriptor;
};

} // namespace

#endif //AX_RENDERER_DX12
