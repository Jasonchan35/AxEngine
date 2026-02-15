module;

export module AxRender:RenderObjectManager_Backend;
export import :RenderSystem_Backend;
export import :Material_Backend;
export import :GpuBuffer_Backend;
export import :RenderObjectTable;
export import :MeshObject;
export import :RenderObjectManager;

export namespace ax /*::AxRender*/ {

class MeshObject_Backend : public MeshObject {
	AX_RTTI_INFO(MeshObject_Backend, MeshObject)
public:
	MeshObject_Backend(const CreateDesc& desc) : Base(desc) {}
};

class RenderObjectManager_CreateDesc {
public:
};

class RenderObjectManager_Backend : public RenderObjectManager {
	AX_RTTI_INFO(RenderObjectManager_Backend, RenderObjectManager)
public:
	AX_DOWNCAST_GET_INSTANCE();
	using CreateDesc = RenderObjectManager_CreateDesc;
	RenderObjectManager_Backend(const CreateDesc& desc);

	static void s_create(const MemAllocRequest& req);
	static void s_destroy();

	void onFrameBegin(RenderRequest_Backend* req);

	void onFrameEnd(RenderRequest_Backend* req);

	template<class T, class CREATE_DESC = typename T::CreateDesc, class RESOURCE_KEY = typename T::ResourceKey>
	bool getOrNewObject(SPtr<T> & sp, const MemAllocRequest& req, const CREATE_DESC& desc, const RESOURCE_KEY& key);

	void onFileChanged(FileDirWatcher_Result& result);
	void hotReloadFile(StrView filename);

	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<Sampler  >>& list) {}
	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<Texture2D>>& list) {}

#if AX_RENDER_BINDLESS
	struct Bindless {
		const ShaderParamSpace_Backend::SamplerParam*	AxBindless_SamplerState = nullptr;
		const ShaderParamSpace_Backend::TextureParam*	AxBindless_Texture3D = nullptr;
		const ShaderParamSpace_Backend::TextureParam*	AxBindless_Texture2D = nullptr;
	} bindless;
#endif
	
	struct StructuredBufferPoolParam {
		SPtr<GpuBufferPool_Backend>	pool;
		void onGpuUpdatePages(RenderRequest_Backend* req) {
			if (pool) pool->onGpuUpdatePages(req);
		}
	};
	
	struct BufferPools {
		SPtr<GpuBufferPool_Backend>	vertex;
		SPtr<GpuBufferPool_Backend>	index;
		SPtr<GpuBufferPool_Backend>	constBuffer;
		
		template<class FUNC>
		void visitPools(FUNC func) {
			func(vertex       );
			func(index        );
			func(constBuffer  );
		}		
	} _bufferPools;

	Material_Backend* globalCommonMaterial() { return _globalCommonMaterial.ptr(); }
	Material_Backend* indirectDrawMaterial() { return _indirectDrawMaterial.ptr(); }
	
	RenderObjectTableBase* getTable(Rtti* rtti) {
		auto* p = _objectTables.scopedLock()->dict.find(rtti);
		return p ? *p : nullptr;
	}
	
	void addTable(RenderObjectTableBase* table) {
		auto lock = _objectTables.scopedLock();
		lock->dict.add(table->objRtti(), table);
	}
	
protected:
	void _postCreate();
	virtual void onPostCreate() {}

	struct ObjectTables {
		Dict<Rtti*, SPtr<RenderObjectTableBase>> dict;
	};
	MutexProtected<ObjectTables> _objectTables;
	
// objects - must after _objectTable
	SPtr<Material_Backend>	_globalCommonMaterial;
	SPtr<Material_Backend>	_indirectDrawMaterial;
};

template<class T, class CREATE_DESC, class RESOURCE_KEY>
bool RenderObjectManager_Backend::getOrNewObject(SPtr<T>&                 sp,
                                                   const MemAllocRequest& req,
                                                   const CREATE_DESC&     desc,
                                                   const RESOURCE_KEY&    key
) {
	if (key) {
		auto table = decltype(T::objectSlot)::Table::s_instance();
		if (auto* p = table->findObject(key)) {
			sp = rttiCastCheck<T>(p);
			return false; // return exists one
		}
	}

	UPtr<T> u;
	RenderSystem_Backend::s_instance()->_newObject(u, req, desc);
	sp = SPtr_fromUPtr(std::move(u));
	return true; // return new one
}
 

} // namespace
