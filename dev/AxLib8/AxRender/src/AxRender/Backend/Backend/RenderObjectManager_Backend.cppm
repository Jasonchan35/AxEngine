module;

export module AxRender:RenderObjectManager_Backend;
export import :RenderSystem_Backend;
export import :Material_Backend;
export import :MeshObject_Backend;
export import :GpuBuffer_Backend;
export import :RenderObjectTable;

export namespace ax /*::AxRender*/ {

class RenderObjectManager_CreateDesc {
public:
};

class RenderObjectManager_Backend : public RenderObject {
	AX_RTTI_INFO(RenderObjectManager_Backend, RenderObject)
public:
	using CreateDesc = RenderObjectManager_CreateDesc;
	RenderObjectManager_Backend(const CreateDesc& desc);

	static RenderObjectManager_Backend* s_instance();
	static void s_create(const MemAllocRequest& req);
	static void s_destroy();

	void onFrameBegin(RenderRequest_Backend* req);

	void onFrameEnd(RenderRequest_Backend* req);

	template<class T, class CREATE_DESC = typename T::CreateDesc, class RESOURCE_KEY = typename T::ResourceKey>
	bool getOrNewResource(SPtr<T> & sp, const MemAllocRequest& req, const CREATE_DESC& desc, const RESOURCE_KEY& key);

	void onFileChanged(FileDirWatcher_Result& result);
	void hotReloadFile(StrView filename);

	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<  Sampler_Backend*>& list) {}
	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<Texture2D_Backend*>& list) {}

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

	struct StructBufferPools {
		StructuredGpuBufferPool_<AxGpuMeshlet>		axMeshlet;
		StructuredGpuBufferPool_<AxGpuMeshletVert>	axMeshletVert;
		StructuredGpuBufferPool_<AxGpuMeshletPrim>	axMeshletPrim;
		
		template<class FUNC>
		void visitPools(FUNC func) {
			func(axMeshlet    );
			func(axMeshletVert);
			func(axMeshletPrim);
		}		
	} _structBufferPools;
	
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
bool RenderObjectManager_Backend::getOrNewResource(SPtr<T>&               sp,
                                                   const MemAllocRequest& req,
                                                   const CREATE_DESC&     desc,
                                                   const RESOURCE_KEY&    key
) {
	if (key) {
		auto table = RenderObjectTable<T>::s_instance();
		if (auto* p = table->findObject(key)) {
			sp = p;
			return false; // return exists one
		}
	}

	UPtr<T> u;
	RenderSystem_Backend::s_instance()->_newObject(u, req, desc);
	sp = SPtr_fromUPtr(std::move(u));
	return true; // return new one
}
 

} // namespace
