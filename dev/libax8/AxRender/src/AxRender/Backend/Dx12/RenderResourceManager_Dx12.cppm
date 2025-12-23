module;

export module AxRender:RenderResourceManager_Dx12;
#if AX_RENDERER_DX12

export import :Dx12Util;
export import :RenderResourceManager_Backend;
export import :Renderer_Backend;

namespace ax {

template<class T>
class RenderResourceTable_Dx12 : public RenderResourceTable_Backend<T> {
public:
	
};

class RenderResourceManager_Dx12 : public RenderResourceManager_Backend {
	AX_RTTI_INFO(RenderResourceManager_Dx12, RenderResourceManager_Backend)
public:
	RenderResourceManager_Dx12(const CreateDesc& desc) : Base(desc) {}

	template<class T> using Table_Dx12 = RenderResourceTable_Dx12<T>;

protected:

	template<class T>
	void newTableIfNull_Dx12(const MemAllocRequest& allocaReq) {
		auto lock = getTable<T>().scopedLock();
		if (lock->get()) return;
		lock->move(UPtr_new<Table_Dx12<T>>(allocaReq));
	}
	
	virtual void onCreated(const CreateDesc& desc) override {
		newTableIfNull_Dx12<Texture2D_Backend>(AX_ALLOC_REQ);
	}
};

}

#endif // #if AX_RENDERER_DX12