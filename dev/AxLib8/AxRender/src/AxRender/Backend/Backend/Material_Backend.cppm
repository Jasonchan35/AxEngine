module;
export module AxRender:Material_Backend;
export import :Shader_Backend;
export import :Texture_Backend;
export import :Material;

export namespace ax /*::AxRender*/ {

/*
    +--------------------------+     +-------------------------+
    |  Shader                  |<-+  | Material                |
    +--------------------------+  |  +-------------------------+
    |                          |  +--| .Shader                 |
    |                          |     |                         |
    | .Passes[]                |<----| .Passes[]               |
    +--+-----------------------+     +--+----------------------+
       |                                |                       
    +--v-----------------------+     +--v----------------------+
    | ShaderPass               |<-+  | Material Pass           |
    +--------------------------+  |  +-------------------------+
    | .Vs / Ps / Cs            |  +--| .Shader RenderPass      |
    |                          |     |                         |      +--------------------------------+
    | .ParamSpaces[Default]    |<----| .ParamSpaces[Default]   |      | Global Common Material Pass 0  |
    |                          |     |                         |      +--------------------------------+
    |                          |     |                         |      | .ParamSpace[Global]            |
    |                          |     |                    Bind +--+-->| .ParamSpace[PerFrame]          |
    |                          |     |                         |  |   +--------------------------------+
    +--------------------------+     +-------------------------+  |   
                                                                  |   +--------------------------------+
                                                                  |   | Per Object                     |
                                                                  |   +--------------------------------+
                                                                  +-->| .ParamSpace[PerObject]         |
                                                                      +--------------------------------+

*/

class MaterialPass_Backend;
class Material_Backend;
class AxDrawCallDesc;

class MaterialParamSpace_CreateDesc : public NonCopyable {
public:
	MaterialPass_Backend* materialPass = nullptr;
	const ShaderParamSpace_Backend*	shaderParamSpace = nullptr;
};

class MaterialParamSpace_Backend : public RenderObject {
	AX_RTTI_INFO(MaterialParamSpace_Backend, RenderObject)
public:
	using CreateDesc = MaterialParamSpace_CreateDesc;

	static SPtr<MaterialParamSpace_Backend> s_new(MaterialPass_Backend*           pass,
	                                              const ShaderParamSpace_Backend* shaderParamSpace); 
	
	MaterialParamSpace_Backend(const CreateDesc& desc);

	using VarInfo	= ShaderParamSpace_Backend::VarInfo;
	using BindPoint = ShaderParamBindPoint;
	using BindSpace = ShaderParamBindSpace;
	using BindCount = ShaderParamBindCount;
	using ParamIndex = ShaderParamSpace_Backend::ParamIndex;

	bool isGlobalCommonShader() const { return _shaderParamSpace->isGlobalCommonShader(); }
	
	struct ParamBase {
		void create(const ShaderParamSpace_Backend::ParamBase& shaderParam) {}
	};

	struct ConstBufferParam : public ParamBase {
		NameId		name() const { return _shaderParam->name(); }
		BindPoint	bindPoint() const { return _shaderParam->bindPoint(); }
		BindCount	bindCount() const { return _shaderParam->bindCount(); }

		void		create(const ShaderParamSpace_Backend::ConstBufferParam& shaderParam);
		Int			dataSize() const { return _dynamicGpuBuffer.dataSize(); }

		const GpuBuffer* getUploadedGpuBuffer(class RenderRequest* req) const {
			return _dynamicGpuBuffer.getUploadedGpuBuffer(req);
		}
		
		template<class V> bool setVariable(const VarInfo* varInfo, const V& value);
		template<class V> bool setVariable(NameId name, const V& value);
	private:
		const ShaderParamSpace_Backend::ConstBufferParam* _shaderParam = nullptr;
		DynamicGpuBuffer _dynamicGpuBuffer;
	};

	struct StructuredBufferParam : public ParamBase {
		NameId		name() const { return _shaderParam->name(); }
		BindPoint	bindPoint() const { return _shaderParam->bindPoint(); }
		BindCount	bindCount() const { return _shaderParam->bindCount(); }

		void create(const ShaderParamSpace_Backend::StructuredBufferParam& shaderParam);
		const StructuredGpuBuffer* buffer() const { return _buffer; }
		const GpuBufferPool* bufferPool() const { return _bufferPool; }
		
		void setBuffer(StructuredGpuBuffer* buf) {
			AX_ASSERT(buf->stride() == stride());
			_buffer.ref(buf); _bufferPool.unref();
		}
		
		void setBufferPool(GpuBufferPool* pool) {
			AX_ASSERT(pool->blockAlignment() == stride());
			_bufferPool.ref(pool); _buffer.unref();
		}

		const GpuBuffer* getUploadedGpuBuffer(class RenderRequest* req) const {
			return _buffer ? _buffer->getUploadedGpuBuffer(req) : nullptr;
		}
		
		Int stride() const { return _shaderParam->stride(); }

	private:
		const ShaderParamSpace_Backend::StructuredBufferParam* _shaderParam = nullptr;
		SPtr<const StructuredGpuBuffer>	_buffer;
		SPtr<const GpuBufferPool> _bufferPool;
	};	

	struct TextureParam : public ParamBase {
		NameId         name() const { return _shaderParam->name(); }
		BindPoint      bindPoint() const { return _shaderParam->bindPoint(); }
		RenderDataType dataType() const { return _shaderParam->dataType(); }
		BindCount      bindCount() const { return _shaderParam->bindCount(); }
		
		void			create(const ShaderParamSpace_Backend::TextureParam& shaderParam);
		const Texture*	texture() const { return _texture; }
		template<class TEX> bool setTexture(const TEX* texture);
	private:
		const ShaderParamSpace_Backend::TextureParam* _shaderParam = nullptr;
		SPtr<const Texture>	_texture;
	};

	struct SamplerParam : public ParamBase {
		NameId		name() const { return _shaderParam->name(); }
		BindPoint	bindPoint() const { return _shaderParam->bindPoint(); }
		BindCount	bindCount() const { return _shaderParam->bindCount(); }

		void			create(const ShaderParamSpace_Backend::SamplerParam& shaderParam);
		const Sampler*	sampler() const { return _sampler; }
		bool			setSampler(const Sampler* sampler) { _sampler = sampler; return true; }
	private:
		const ShaderParamSpace_Backend::SamplerParam* _shaderParam = nullptr;
		SPtr<const Sampler>		_sampler;
	};

	bool setParam(NameId name, const i32&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec1i32&	v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec2i32&	v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec3i32&	v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec4i32&	v) { return _setVariable(name, v); }

	bool setParam(NameId name, const u32&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec1u32&	v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec2u32&	v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec3u32&	v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec4u32&	v) { return _setVariable(name, v); }

	bool setParam(NameId name, const f32&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec1f&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec2f&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec3f&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec4f&		v) { return _setVariable(name, v); }

	bool setParam(NameId name, const f64&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec1d&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec2d&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec3d&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Vec4d&		v) { return _setVariable(name, v); }

	bool setParam(NameId name, const Mat4f&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Mat4d&		v) { return _setVariable(name, v); }

	bool setParam(NameId name, const Color3f&	v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Color4f&	v) { return _setVariable(name, v); }

	bool setParam(NameId name, Sampler*		v);
	bool setParam(NameId name, Texture2D*	v);
	
	bool setParam(NameId name, StructuredGpuBuffer* v);
	
	BindSpace bindSpace() const { return _shaderParamSpace->bindSpace(); }

	TempString debugName() const { return _shaderParamSpace ? _shaderParamSpace->debugName() : ""; }
	
	const ShaderParamSpace_Backend* shaderParamSpace_backend() const { return _shaderParamSpace.ptr(); }

	ConstBufferParam*      findConstBufferParam(NameId name)		{ return _findParam(_constBufferParams, name); }
	StructuredBufferParam* findStructuredBufferParam(NameId name)	{ return _findParam(_structuredBufferParams, name); }
	TextureParam*          findTextureParam(NameId name)			{ return _findParam(_textureParams, name); }
	SamplerParam*          findSamplerParam(NameId name)			{ return _findParam(_samplerParams, name); }
	
	Array<ConstBufferParam,      1>	_constBufferParams;
	Array<StructuredBufferParam, 1>	_structuredBufferParams;
	Array<TextureParam,          2>	_textureParams;
	Array<SamplerParam,          2>	_samplerParams;

	const MaterialPass_Backend* materialPass() const { return _materialPass; };

	ConstBufferParam* constBuffer_globals() { return _constBuffer_globals; }
	ConstBufferParam* constBuffer_camera () { return _constBuffer_camera ; }
	
protected:
	friend class MaterialPass_Backend;
	MaterialPass_Backend* _materialPass = nullptr;
	SPtr<const ShaderParamSpace_Backend> _shaderParamSpace;
	
	ConstBufferParam* _constBuffer_globals = nullptr;
	ConstBufferParam* _constBuffer_camera  = nullptr;

	template<class T> T* _findParam(IArray<T>& arr, NameId name) {
		return arr.find_([&name](const T& e) { return e.name() == name; });
	}
	template<class V> bool	_setVariable(NameId name, const V& v);
//	template<class V> bool	_setTextureParam(NameId name, V* v);
};

template<class V> AX_INLINE
bool MaterialParamSpace_Backend::_setVariable(NameId name, const V& v) {
#if 1
	auto r = _shaderParamSpace->findVarInfo(name);
	if (!r) return false;
	return _constBufferParams[r->constBufferIndex].setVariable(r->varInfo, v);
	
#else
	bool b = false;
	for (auto& cb : _constBuffers) {
		b = b || cb.setVariable(name, v);
	}
	return b;

#endif
}

template<class V> inline
bool MaterialParamSpace_Backend::ConstBufferParam::setVariable(NameId name, const V& value) {
	if (!_shaderParam) return false;
	return setVariable(_shaderParam->findVarInfo(name), value);
}

template<class V> inline
bool MaterialParamSpace_Backend::ConstBufferParam::setVariable(const VarInfo* varInfo, const V& value) {
	if (!varInfo) return false;
	if (varInfo->dataType() != RenderDataType_get<V>) throw Error_Undefined();
	auto range = varInfo->assignValueToBuffer(_dynamicGpuBuffer.mutSpan(), value);
	_dynamicGpuBuffer.markDirty(range);
	return true;
}

template<class TEX> inline
bool MaterialParamSpace_Backend::TextureParam::setTexture(const TEX* texture) {
	if (dataType() != RenderDataType_get<TEX>) {
		return false;
	}
	_texture.ref(texture);
	return true;
}

class MaterialPass_CreateDesc : public NonCopyable {
public:
	Material_Backend* material = nullptr;
	ShaderPass_Backend* shaderPass = nullptr;
	Int passIndex = 0;
};

class MaterialPass_Backend : public RenderObject {
	AX_RTTI_INFO(MaterialPass_Backend, RenderObject)
public:
	using CreateDesc = MaterialPass_CreateDesc;
	using BindPoint = ShaderParamBindPoint;
	using BindCount = ShaderParamBindCount;
	using BindSpace = ShaderParamBindSpace;
	static constexpr auto BindSpace_COUNT = ax_enum_int(BindSpace::_COUNT);

	MaterialPass_Backend(const CreateDesc& desc);
	virtual ~MaterialPass_Backend() override;

	static MaterialPass_Backend* s_globalCommonMaterialPass();

	const ShaderPass_Backend*	shaderPass() const { return _shaderPass; }
	const Shader_Backend*		shader() const;

	virtual bool onBindMaterial(class RenderRequest* req, AxDrawCallDesc& cmd) = 0;

	bool isOwnParamSpace(BindSpace s) const { return _shaderPass->isOwnParamSpace(s); }
	bool isMeshShader() const { return _shaderPass->isMeshShader(); }
	bool isComputeShader() const { return _shaderPass->isComputeShader(); }

	NameId getPropSamplerName(NameId name) const { auto* shd = shader(); return shd ? shd->getPropSamplerName(name) : NameId(); }

	bool isGlobalCommonShader() const { return _shaderPass->isGlobalCommonShader(); }
	
	void logWarningOnce(StrView msg);

	const MaterialParamSpace_Backend* getParamSpace(BindSpace s) const {
		auto* pp = _materialParamSpaces.tryGetElement(ax_enum_int(s));
		return pp ? pp->ptr() : nullptr;
	}

	MaterialParamSpace_Backend* getOwnParamSpace(BindSpace s) {
		if (!isOwnParamSpace(s)) return nullptr;
		return ax_const_cast(getParamSpace(s));
	}
	
	template<class V>
	bool setParam(BindSpace space, NameId name, const V& v) {
		auto* p = ax_const_cast(getOwnParamSpace(space));
		return p ? p->setParam(name, v) : false;
	}

	virtual void onSetShader() {}
	Int maxFrameDataCount() const;

	TempString debugName() const { return _shaderPass ? _shaderPass->debugName() : ""; }

	      Material_Backend*   material()       { return _material; }
	const Material_Backend*   material() const { return _material; }
protected:
	Material_Backend*   _material          = nullptr;
	ShaderPass_Backend* _shaderPass        = nullptr;
	Int                 _passIndex         = 0;

friend class Material_Backend;
private:
	FixedArray<SPtr<const MaterialParamSpace_Backend>, BindSpace_COUNT>	_materialParamSpaces;
	template<class T> static T* _findParam(IArray<T>& arr, NameId name);
};

class Material_Backend : public Material {
	AX_RTTI_INFO(Material_Backend, Material)
public:
	Material_Backend(const CreateDesc& desc);
	
	static Material_Backend* s_globalCommonMaterial();

	static SPtr<This> s_new(const MemAllocRequest& req, Shader* shader = nullptr);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView shaderAssetPath);

	void setShader_backend(Shader* shader);
	Shader_Backend* shader_backend() { return _shader_backend; }

	template<class V> AX_INLINE
	bool setParam(BindSpace space, NameId name, V& v);

	Int		passCount() const { return _passes.size(); }

	MaterialPass_Backend*	getPass(Int index);

	StrView shaderAssetPath() const { return _shader_backend ? StrView(_shader_backend->assetPath()) : StrView(); }

	const MaterialParamSpace_Backend*	getPassParamSpace(Int passIndex, BindSpace s) const {
		auto* pp = _passes.tryGetElement(passIndex);
		auto* p  = pp ? pp->ptr() : nullptr;
		return p ? p->getParamSpace(s) : nullptr;
	}

	template<class R> const R* getPassParamSpace_(Int pass, BindSpace s) const {
		return rttiCastCheck<R>(getPassParamSpace(pass, s));
	}

	void logWarningOnce(StrView msg);
	Int maxFrameDataCount() const { return _maxFrameDataCount; }

	TempString debugName() const { return _shader_backend ? _shader_backend->debugName() : ""; }
	
protected:
	friend class MaterialPass_Backend;
	
	bool _bShowWarning = true;
	Int _maxFrameDataCount = 0;
	
	Array<UPtr<MaterialPass_Backend>>	_passes;
	SPtr<Shader_Backend>	_shader_backend;
	virtual void onSetShader();

	virtual UPtr<MaterialPass_Backend>	onNewPass(const MaterialPass_CreateDesc& desc) = 0;
};

AX_INLINE
const Shader_Backend* MaterialPass_Backend::shader() const {
	auto* m = _material;
	return m ? m->shader_backend() : nullptr;
}

template<class V> AX_INLINE 
bool Material_Backend::setParam(BindSpace space, NameId name, V& v) {
	bool b = false;
	for (auto& p : _passes) {
		if (p) { b = b || p->setParam(space, name, v); }
	}
	return b;
}

template<class T> AX_INLINE
T* MaterialPass_Backend::_findParam(IArray<T>& arr, NameId name) {
	for (auto& e : arr) {
		if (e.name() == name) return &e;
	}
	return nullptr;
}

AX_INLINE
MaterialPass_Backend* Material_Backend::getPass(Int index) {
	if (index >= _passes.size()) {
		AX_ASSERT(false);
		return nullptr;
	}
	return _passes[index].get();
}

AX_INLINE Int MaterialPass_Backend::maxFrameDataCount() const { return _material->maxFrameDataCount(); }

} // namespace