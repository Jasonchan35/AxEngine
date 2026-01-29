module;

export module AxRender:RenderObjectManager_Backend;
export import :RenderSystem_Backend;
export import :Material_Backend;
export import :MeshObject_Backend;
export import :GpuBuffer_Backend;

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

	template<class T>
	using Table = RenderObjectTable_Backend<T>;

	template<class T>
	AX_NODISCARD MutexProtected<Table<T>>& getTable() {
		using TABLE = MutexProtected<Table<T>>; 
		return _objectTables.get<TABLE>();
	}

	auto& table_shader()	{ return getTable<Shader_Backend>(); }
	auto& table_sampler()	{ return getTable<Sampler_Backend>(); }
	auto& table_texture2D()	{ return getTable<Texture2D_Backend>(); }

	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<   Sampler_Backend>>& list) {}
	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr< Texture2D_Backend>>& list) {}
	virtual void onUpdateMeshObject (RenderRequest_Backend* req, Array<SPtr<MeshObject_Backend>>& list) {}

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
		
		SPtr<GpuBufferPool_Backend> axMeshlet;
		SPtr<GpuBufferPool_Backend> axMeshletVert;
		SPtr<GpuBufferPool_Backend> axMeshletPrim;
		
		template<class FUNC>
		void visit(FUNC func) {
			func(vertex       );
			func(index        );
			func(constBuffer  );
			func(axMeshlet    );
			func(axMeshletVert);
			func(axMeshletPrim);
		}		
	} _bufferPools;

	Material_Backend* globalCommonMaterial() { return _globalCommonMaterial.ptr(); }
	Material_Backend* indirectDrawMaterial() { return _indirectDrawMaterial.ptr(); }
	
protected:
	void _postCreate();
	virtual void onPostCreate() {}

	template<class FUNC>
	void visitObjectTable(FUNC func) {
		_objectTables.apply([&func](auto&... list) {
			(func(list),...);
		});
	}

	using ObjectTables = Tuple<
		MutexProtected<Table< Shader_Backend     >>,
		MutexProtected<Table< Sampler_Backend    >>,
		MutexProtected<Table< Texture2D_Backend  >>,
		MutexProtected<Table< MeshObject_Backend >>
	>;
	ObjectTables _objectTables;
	
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
		auto table = getTable<T>().scopedLock();
		if (auto* p = table->findObject(key)) {
			sp = p;
			return false;
		}
	}

	UPtr<T> u;
	RenderSystem_Backend::s_instance()->_newObject(u, req, desc);
	sp = SPtr_fromUPtr(std::move(u));
	return true;
}

} // namespace
