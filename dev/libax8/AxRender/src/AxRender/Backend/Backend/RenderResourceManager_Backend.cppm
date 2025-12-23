module;
export module AxRender:RenderResourceManager_Backend;
export import :Texture_Backend;
export import :Shader_Backend;
export import :Renderer_Backend;

export namespace ax /*::AxRender*/ {

class RenderResourceManager_CreateDesc {
	
};

class RenderResourceManager_Backend : public RenderObject {
	AX_RTTI_INFO(RenderResourceManager_Backend, RenderObject)
public:
	using CreateDesc = RenderResourceManager_CreateDesc;
	
	static RenderResourceManager_Backend* s_instance();
	static void s_create(const MemAllocRequest& req);
	static void s_destroy();

	void onFrameBegin(RenderRequest_Backend* req);

	void onFrameEnd(RenderRequest_Backend* req);

	template<class T, class CREATE_DESC = typename T::CreateDesc, class RESOURCE_KEY = typename T::ResourceKey>
	bool getOrNewResource(SPtr<T> & sp, const MemAllocRequest& req, const CREATE_DESC& desc, const RESOURCE_KEY& key);

	void onFileChanged(FileDirWatcher_Result& result);

	void hotReloadFile(StrView filename);

	using ShaderTable      = RenderResourceTable_Backend<Shader_Backend>;
	using SamplerTable     = RenderResourceTable_Backend<Sampler_Backend>;
	using Texture2DTable   = RenderResourceTable_Backend<Texture2D_Backend>;

	MutexProtected<ShaderTable     > shaderTable;
	MutexProtected<SamplerTable    > samplerTable;
	MutexProtected<Texture2DTable  > texture2DTable;
	
	void getTable(MutexProtected<ShaderTable     >* & o) { o = &shaderTable; }
	void getTable(MutexProtected<SamplerTable    >* & o) { o = &samplerTable; }
	void getTable(MutexProtected<Texture2DTable  >* & o) { o = &texture2DTable; }
	
	template<class FUNC>
	void visit(FUNC func) {
		func(shaderTable);
		func(samplerTable);
		func(texture2DTable);
	}

protected:
	RenderResourceManager_Backend(const CreateDesc& desc) {}
};

template<class T, class CREATE_DESC, class RESOURCE_KEY>
bool RenderResourceManager_Backend::getOrNewResource(SPtr<T>&               sp,
                                                     const MemAllocRequest& req,
                                                     const CREATE_DESC&     desc,
                                                     const RESOURCE_KEY&    key
) {
	MutexProtected<RenderResourceTable_Backend<T>>* table = nullptr;
	getTable(table);

	auto tableLock = table->scopedLock();
	if (auto* p = tableLock->findObject(key)) {
		sp = p;
		return false;
	}

	UPtr<T> u;
	Renderer_Backend::s_instance()->_newObject(u, req, desc);
	sp = SPtr_fromUPtr(std::move(u));
	return true;
}

} // namespace
