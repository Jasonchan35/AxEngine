module;
export module AxRender:ResourceManager_Backend;
export import :Texture_Backend;
export import :Shader_Backend;
export import :Renderer_Backend;

export namespace ax::AxRender {

class ResourceManager_Backend : public NonCopyable {
	using This = ResourceManager_Backend;
public:
	static ResourceManager_Backend* s_instance();
	static void s_create(const MemAllocRequest& req);
	static void s_destroy();

	void onFrameEnd(RenderRequest_Backend* req) {
		visit([&](auto& table){
			table.scopedLock()->onFrameEnd(req);
		});
	}

	template<class T, class CreateDesc = typename T::CreateDesc, class ResourceKey = typename T::ResourceKey>
	bool getOrNewResource(SPtr<T> & sp, const MemAllocRequest& req, const CreateDesc& desc, const ResourceKey& key);

	void onFileChanged(FileDirWatcher_Result& result);

	void hotReloadFile(StrView filename);

	using ShaderTable    = ResourceTable_Backend<Shader_Backend  >;
	using SamplerTable   = ResourceTable_Backend<Sampler_Backend  >;
	using Texture2DTable = ResourceTable_Backend<Texture2D_Backend>;
	using Texture3DTable = ResourceTable_Backend<Texture3D_Backend>;

	void getTable(Thread::MutexProtected<ShaderTable   >* & o) { o = &shaderTable; }
	void getTable(Thread::MutexProtected<SamplerTable  >* & o) { o = &samplerTable; }
	void getTable(Thread::MutexProtected<Texture2DTable>* & o) { o = &texture2DTable; }
	void getTable(Thread::MutexProtected<Texture3DTable>* & o) { o = &texture3DTable; }

	Thread::MutexProtected<ShaderTable   > shaderTable;
	Thread::MutexProtected<SamplerTable  > samplerTable;
	Thread::MutexProtected<Texture2DTable> texture2DTable;
	Thread::MutexProtected<Texture3DTable> texture3DTable;

	template<class FUNC>
	void visit(FUNC func) {
		func(shaderTable);
		func(samplerTable);
		func(texture2DTable);
		func(texture3DTable);
	}

};

template<class T, class CreateDesc, class ResourceKey>
bool ResourceManager_Backend::getOrNewResource(SPtr<T>&			   sp,
											   const MemAllocRequest& req,
											   const CreateDesc&   desc,
											   const ResourceKey&  key) {
	Thread::MutexProtected<ResourceTable_Backend<T>>* table = nullptr;
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
