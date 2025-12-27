module;

export module AxRender:RenderResourceManager_Dx12;
#if AX_RENDERER_DX12

export import :Dx12DescriptorHeap;
export import :RenderResourceManager_Backend;
export import :RenderSystem_Backend;

namespace ax {

template<class HEAP_CHUNK>
class ResourceDescriptorHeap : public NonCopyable {
public:
	using HeapPool = typename HEAP_CHUNK::HeapPool;

	void create(StrView name, HeapPool& gpuDescriptorHeapPool, Int numDescriptors) {
		auto* dev = gpuDescriptorHeapPool.d3dDevice();
		_pool.create(Fmt("{}-pool", name), dev, numDescriptors, false);
		_chunk.create(Fmt("{}-chunk", name), _pool, numDescriptors, true);
#if AX_RENDER_BINDLESS
		_bindless.create(Fmt("{}-bindless", name), gpuDescriptorHeapPool, numDescriptors, true);
#endif
	}

	ID3D12DescriptorHeap* d3dHeap() { return _pool.d3dHeap(); }
	
#if AX_RENDER_BINDLESS
	HEAP_CHUNK& bindless()  { return _bindless; }
#endif
	
protected:
	HeapPool	_pool;  // Own pool for CPU
	HEAP_CHUNK	_chunk; // CPU write / GPU read
#if AX_RENDER_BINDLESS
	HEAP_CHUNK  _bindless; // GPU only
#endif
};

struct ResourceDescriptorHeap_CBV_SRV_UAV : public ResourceDescriptorHeap<Dx12DescriptorHeapChunk_CBV_SRV_UAV> {
	Dx12Descriptor_Texture2D setTexture(Int index, const Dx12Resource_Texture2D& res) {
#if AX_RENDER_BINDLESS
		auto dp = _chunk.setTexture(index, res);
		return _bindless.setDescriptor(index, dp);
#else
		return _chunk.setTexture(index, res);
#endif
	}
};


using ResourceDescriptorHeap_Sampler     = ResourceDescriptorHeap<Dx12DescriptorHeapChunk_Sampler    >;

class RenderResourceManager_Dx12 : public RenderResourceManager_Backend {
	AX_RTTI_INFO(RenderResourceManager_Dx12, RenderResourceManager_Backend)
public:
	AX_DOWNCAST_GET_INSTANCE();

	using BindSpace = ShaderParamBindSpace;
	
	RenderResourceManager_Dx12(const CreateDesc& desc);

	struct DescriptorHeapPools : public NonCopyable {
		Dx12DescriptorHeapPool_ColorBuffer	ColorBuffer;
		Dx12DescriptorHeapPool_DepthBuffer	DepthBuffer;
		Dx12DescriptorHeapPool_CBV_SRV_UAV	CBV_SRV_UAV;
		Dx12DescriptorHeapPool_Sampler		Sampler;
	} descriptorHeapPools;

	struct ResourceDescriptors : public NonCopyable {
		ResourceDescriptorHeap_CBV_SRV_UAV	Texture2D;
		ResourceDescriptorHeap_CBV_SRV_UAV	Texture3D;
		ResourceDescriptorHeap_Sampler		Sampler;
	} resourceDescriptors;
	
	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<Sampler_Backend  >>& list) override;
	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<Texture2D_Backend>>& list) override;
	virtual void onPostCreate() override;
	
#if AX_RENDER_BINDLESS
	struct BindlessDescriptors : public NonCopyable {
		Dx12DescriptorHandle	CBV_SRV_UAV;
		Dx12DescriptorHandle	Sampler;
	} bindlessDescriptors;
#endif	
};

} // namespace
#endif // #if AX_RENDERER_DX12