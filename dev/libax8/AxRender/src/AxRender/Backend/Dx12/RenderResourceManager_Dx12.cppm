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

	void create(Dx12_ID3D12Device* dev, Int numDescriptors) {
		_heapPool.create(dev, numDescriptors, false);
		_heapChunk.create(_heapPool, numDescriptors);
		_heapChunk.adjustUsedToSize();
	}

	HEAP_CHUNK* operator->() { return &_heapChunk; }

	ID3D12DescriptorHeap* d3dHeap() { return _heapPool.d3dHeap(); }

	HEAP_CHUNK& heapChunk() { return _heapChunk; }
	
private:
	HeapPool	_heapPool;
	HEAP_CHUNK	_heapChunk;
};

using ResourceDescriptorHeap_CBV_SRV_UAV = ResourceDescriptorHeap<Dx12DescriptorHeapChunk_CBV_SRV_UAV>;
using ResourceDescriptorHeap_Sampler     = ResourceDescriptorHeap<Dx12DescriptorHeapChunk_Sampler    >;

class RenderResourceManager_Dx12 : public RenderResourceManager_Backend {
	AX_RTTI_INFO(RenderResourceManager_Dx12, RenderResourceManager_Backend)
public:
	AX_DOWNCAST_GET_INSTANCE();
	
	RenderResourceManager_Dx12(const CreateDesc& desc);

	Dx12DescriptorHeapPool_ColorBuffer	descHeapPool_ColorBuffer;
	Dx12DescriptorHeapPool_DepthBuffer	descHeapPool_DepthBuffer;
	Dx12DescriptorHeapPool_CBV_SRV_UAV	descHeapPool_CBV_SRV_UAV;
	Dx12DescriptorHeapPool_Sampler		descHeapPool_Sampler;

	struct ResourceDescriptor {
		ResourceDescriptorHeap_CBV_SRV_UAV	Texture2D;
		ResourceDescriptorHeap_Sampler		Sampler;
	} resourceDesc;
	
};

} // namespace
#endif // #if AX_RENDERER_DX12