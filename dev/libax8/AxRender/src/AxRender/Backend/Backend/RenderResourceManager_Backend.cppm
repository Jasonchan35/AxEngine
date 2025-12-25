module;
export module AxRender:RenderResourceManager_Backend;
export import :Texture_Backend;
export import :Shader_Backend;
export import :RenderSystem_Backend;

export namespace ax /*::AxRender*/ {

class RenderResourceManager_CreateDesc {
public:
};

class RenderResourceManager_Backend : public RenderObject {
	AX_RTTI_INFO(RenderResourceManager_Backend, RenderObject)
public:
	using CreateDesc = RenderResourceManager_CreateDesc;
	RenderResourceManager_Backend(const CreateDesc& desc) {}
	
	static RenderResourceManager_Backend* s_instance();
	static void s_create(const MemAllocRequest& req);
	static void s_destroy();

	void onFrameBegin(RenderRequest_Backend* req);

	void onFrameEnd(RenderRequest_Backend* req);

	template<class T, class CREATE_DESC = typename T::CreateDesc, class RESOURCE_KEY = typename T::ResourceKey>
	bool getOrNewResource(SPtr<T> & sp, const MemAllocRequest& req, const CREATE_DESC& desc, const RESOURCE_KEY& key);

	void onFileChanged(FileDirWatcher_Result& result);

	void hotReloadFile(StrView filename);

	template<class T>
	using Table = RenderResourceTable_Backend<T>;

	template<class T>
	AX_NODISCARD MutexProtected<Table<T>>& getTable() {
		using TABLE = MutexProtected<Table<T>>; 
		return _all_table.get<TABLE>();
	}

	auto& table_shader()	{ return getTable<Shader_Backend>(); }
	auto& table_sampler()	{ return getTable<Sampler_Backend>(); }
	auto& table_texture2D()	{ return getTable<Texture2D_Backend>(); }

	virtual void onUpdateDescriptors(Array<SPtr<Sampler_Backend  >>& list) {}
	virtual void onUpdateDescriptors(Array<SPtr<Texture2D_Backend>>& list) {}
	
protected:
	template<class FUNC>
	void visit(FUNC func) {
		_all_table.apply([&func](auto&... list) {
			(func(list),...);
		});
	}

	using All_Table = Tuple<
		MutexProtected<Table<Shader_Backend>   >,
		MutexProtected<Table<Sampler_Backend>  >,
		MutexProtected<Table<Texture2D_Backend>>
	>;
	All_Table _all_table;
};

template<class T, class CREATE_DESC, class RESOURCE_KEY>
bool RenderResourceManager_Backend::getOrNewResource(SPtr<T>&               sp,
                                                     const MemAllocRequest& req,
                                                     const CREATE_DESC&     desc,
                                                     const RESOURCE_KEY&    key
) {
	auto table = getTable<T>().scopedLock();
	if (auto* p = table->findObject(key)) {
		sp = p;
		return false;
	}

	UPtr<T> u;
	RenderSystem_Backend::s_instance()->_newObject(u, req, desc);
	sp = SPtr_fromUPtr(std::move(u));
	return true;
}

} // namespace
