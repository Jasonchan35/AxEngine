module;
export module AxRender:Material_Backend;
export import :Shader_Backend;
export import :Texture_Backend;
export import :Material;

export namespace ax::AxRender {

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
class Cmd_DrawCall;
class Material_Backend;

class MaterialParamSpace_Backend : public MaterialParamSpace {
	AX_RTTI_INFO(MaterialParamSpace_Backend, MaterialParamSpace)
public:
	MaterialParamSpace_Backend(const CreateDesc& desc);

	using VarInfo	= ShaderParamSpace_Backend::VarInfo;

	struct ParamBase {
		void	  create(const ShaderParamSpace_Backend::ParamBase& shaderParam);
	};

	struct ConstBuffer : public ParamBase {
		NameId		name() const { return _shaderParam->name(); }
		BindPoint	bindPoint() const { return _shaderParam->bindPoint(); }
		Int			bindCount() const { return _shaderParam->bindCount(); }

		void		create(const ShaderParamSpace_Backend::ConstBuffer& shaderParam);
		GpuBuffer*	getUploadedGpuBuffer(class RenderRequest* req) { return _dynamicGpuBuffer.getUploadedGpuBuffer(req); }
		Int			dataSize() const { return _dynamicGpuBuffer.dataSize(); }

		template<class V> bool setVariable(const VarInfo* varInfo, const V& value);
		template<class V> bool setVariable(NameId name, const V& value);
	private:
		const ShaderParamSpace_Backend::ConstBuffer* _shaderParam = nullptr;
		DynamicGpuBuffer _dynamicGpuBuffer;
	};

	struct TextureParam : public ParamBase {
		NameId		name() const { return _shaderParam->name(); }
		BindPoint	bindPoint() const { return _shaderParam->bindPoint(); }
		DataType	dataType() const { return _shaderParam->dataType(); }
		Int			bindCount() const { return _shaderParam->bindCount(); }

		void		create(const ShaderParamSpace_Backend::TextureParam& shaderParam);
		Texture*	texture() { return _texture; }
		template<class TEX> bool setTexture(TEX* texture);
	private:
		const ShaderParamSpace_Backend::TextureParam* _shaderParam = nullptr;
		SPtr<Texture>	_texture;
	};

	struct SamplerParam : public ParamBase {
		NameId		name() const { return _shaderParam->name(); }
		BindPoint	bindPoint() const { return _shaderParam->bindPoint(); }
		Int			bindCount() const { return _shaderParam->bindCount(); }

		void		create(const ShaderParamSpace_Backend::SamplerParam& shaderParam);
		Sampler*	sampler() { return _sampler; }
		bool		setSampler(Sampler* sampler) { _sampler = sampler; return true; }
	private:
		const ShaderParamSpace_Backend::SamplerParam* _shaderParam = nullptr;
		SPtr<Sampler>		_sampler;
	};

	struct StorageBufferParam : public ParamBase {
		NameId		name() const { return _shaderParam->name(); }
		BindPoint	bindPoint() const { return _shaderParam->bindPoint(); }
		Int			bindCount() const { return _shaderParam->bindCount(); }

		void		   create(const ShaderParamSpace_Backend::StorageBufferParam& shaderParam);
		StorageBuffer* storageBuffer() { return _storageBuffer; }
		bool		   setStorageParam(StorageBuffer* vb) { _storageBuffer.ref(vb); return true; }
		GpuBuffer*	   gpuBuffer() { return _storageBuffer ? _storageBuffer->gpuBuffer() : nullptr; }
		Int			   dataSize() const { return _storageBuffer ? _storageBuffer->bufferSize() : 0; }

	private:
		const ShaderParamSpace_Backend::StorageBufferParam* _shaderParam = nullptr;
		SPtr<StorageBuffer>									_storageBuffer;
	};

	template<class PARAM>
	static Int s_totalBindCount(Span<PARAM> params) {
		Int total = 0;
		for (auto& param : params) { total += param.bindCount(); }
		return total;
	}

	bool setParam(NameId name, const i32&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const i32x2&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const i32x3&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const i32x4&		v) { return _setVariable(name, v); }

	bool setParam(NameId name, const u32&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const u32x2&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const u32x3&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const u32x4&		v) { return _setVariable(name, v); }

	bool setParam(NameId name, const f32&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const f32x2&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const f32x3&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const f32x4&		v) { return _setVariable(name, v); }

	bool setParam(NameId name, const f64&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const f64x2&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const f64x3&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const f64x4&		v) { return _setVariable(name, v); }

	bool setParam(NameId name, const Mat4f&		v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Mat4d&		v) { return _setVariable(name, v); }

	bool setParam(NameId name, const Color3f&	v) { return _setVariable(name, v); }
	bool setParam(NameId name, const Color4f&	v) { return _setVariable(name, v); }

	bool setParam(NameId name, Sampler*		v);
	bool setParam(NameId name, Texture2D*	v);

	bool setParam(NameId name, StorageBuffer* v);

	Span<ConstBuffer>			constBuffers()	const		{ return _constBuffers;        }
	Span<TextureParam>			textureParams() const		{ return _textureParams;       }
	Span<SamplerParam>			samplerParams() const		{ return _samplerParams;       }
	Span<StorageBufferParam>	storageBufferParams() const	{ return _storageBufferParams; }

	Int constBuffers_totalBindCount() const			{ return s_totalBindCount(_constBuffers.span()); }
	Int textureParams_totalBindCount() const		{ return s_totalBindCount(_textureParams.span()); }
	Int samplerParams_totalBindCount() const		{ return s_totalBindCount(_samplerParams.span()); }
	Int storageBufferParams_totalBindCount() const	{ return s_totalBindCount(_storageBufferParams.span()); }
	
	BindSpace	bindSpace() const { return _shaderParamSpace->bindSpace(); }

	const ShaderParamSpace_Backend* shaderParamSpace_backend() const { return _shaderParamSpace.ptr(); }

protected:
	SPtr<const ShaderParamSpace_Backend> _shaderParamSpace;

	template<class T> T*	_findParam(IArray<T>& arr, NameId name);
	template<class V> bool	_setVariable(NameId name, const V& v);
//	template<class V> bool	_setTextureParam(NameId name, V* v);

	Array<ConstBuffer,        1>	_constBuffers;
	Array<StorageBufferParam, 0>	_storageBufferParams;
	Array<TextureParam,       2>	_textureParams;
	Array<SamplerParam,       2>	_samplerParams;
};

template<class V> AX_INLINE
bool MaterialParamSpace_Backend::_setVariable(NameId name, const V& v) {
	bool b = false;
	for (auto& cb : _constBuffers) {
		b = b || cb.setVariable(name, v);
	}
	return b;
}

template<class T> AX_INLINE
T* MaterialParamSpace_Backend::_findParam(IArray<T>& arr, NameId name) {
	for (auto& e : arr) {
		if (e.name() == name) return &e;
	}
	return nullptr;
}

template<class V> inline
bool MaterialParamSpace_Backend::ConstBuffer::setVariable(NameId name, const V& value) {
	if (!_shaderParam) return false;
	return setVariable(_shaderParam->findVarInfo(name), value);
}

template<class V> inline
bool MaterialParamSpace_Backend::ConstBuffer::setVariable(const VarInfo* varInfo, const V& value) {
	if (!varInfo) return false;
	if (varInfo->dataType() != DataType_get<V>) { AX_ASSERT(false); return false; }
	auto range = varInfo->assignValueToBuffer(_dynamicGpuBuffer.mutSpan(), value);
	_dynamicGpuBuffer.markDirty(range);
	return true;
}

template<class TEX> inline
bool MaterialParamSpace_Backend::TextureParam::setTexture(TEX* texture) {
	if (dataType() != DataType_get<TEX>) {
		return false;
	}
	_texture.ref(texture);
	return true;
}

inline void MaterialParamSpace_Backend::ParamBase::create(const ShaderParamSpace_Backend::ParamBase& shaderParam) {
}

class MaterialPass_Backend_CreateDesc : public NonCopyable {
public:
	Material_Backend* material = nullptr;
	Int passIndex = 0;
};

class MaterialPass_Backend : public RttiObject {
	AX_RTTI_INFO(MaterialPass_Backend, RttiObject)
public:
	using CreateDesc = MaterialPass_Backend_CreateDesc;

	MaterialPass_Backend(const CreateDesc& desc);

	const ShaderPass_Backend*	shaderPass() const;
	const Shader_Backend*		shader() const;

	virtual bool onDrawcall(class RenderRequest* req, Cmd_DrawCall& cmd) = 0;

	NameId getPropSamplerName(NameId name) const { auto* shd = shader(); return shd ? shd->getPropSamplerName(name) : NameId(); }

	void logWarningOnce(StrView msg);

	template<class R>
	R* getParamSpace_(BindSpace s) {
		return rttiCastCheck<R>(getParamSpace(s));
	}

	MaterialParamSpace_Backend* getParamSpace(BindSpace s) {
		auto* pp = _materialParamSpaces.tryGetElement(ax_enum_int(s));
		return pp ? pp->ptr() : nullptr;
	}

	template<class V>
	bool setParamSpaceParam(BindSpace space, NameId name, const V& v) {
		auto* p = getParamSpace(space);
		return p ? p->setParam(name, v) : false;
	}

private:
	Material_Backend*	_material	= nullptr;
	Int _passIndex = 0;

friend class Material_Backend;
protected:
	Array<SPtr<MaterialParamSpace_Backend>>		_materialParamSpaces;

	template<class T> static T* _findParam(IArray<T>& arr, NameId name);
};

class Material_Backend : public Material {
	AX_RTTI_INFO(Material_Backend, Material)
public:
	Material_Backend(const CreateDesc& desc);
	
	static SPtr<This> s_new(const MemAllocRequest& req, Shader* shader = nullptr);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView shaderAssetPath);

	void setShader_backend(Shader* shader);
	Shader_Backend* shader_backend() { return _shader; }

	template<class V> AX_INLINE
	bool setParamSpaceParam(BindSpace space, NameId name, V& v);

	Int		passCount() const { return _passes.size(); }

	MaterialPass_Backend*	getPass(Int index);

	StrView shaderAssetPath() const { return _shader ? StrView(_shader->assetPath()) : StrView(); }

	template<class R> R* getPassParamSpace_(Int pass, BindSpace s) {
		return rttiCastCheck<R>(getPassParamSpace(pass, s));
	}

	MaterialParamSpace_Backend*	getPassParamSpace(Int passIndex, BindSpace s) {
		auto* pp = _passes.tryGetElement(passIndex);
		auto* p  = pp ? pp->ptr() : nullptr;
		return p ? p->getParamSpace(s) : nullptr;
	}

	void logWarningOnce(StrView msg);

friend class MaterialPass_Backend;
protected:

	bool _bShowWarning = true;
	Array<UPtr<MaterialPass_Backend>>	_passes;
	SPtr<Shader_Backend>	_shader;
	virtual void onSetShader() {}

	virtual UPtr<MaterialPass_Backend>	onNewPass(const MaterialPass_Backend_CreateDesc& desc) = 0;
};

AX_INLINE
const Shader_Backend* MaterialPass_Backend::shader() const {
	auto* m = _material;
	return m ? m->shader_backend() : nullptr;
}

AX_INLINE
const ShaderPass_Backend* MaterialPass_Backend::shaderPass() const {
	auto* s = shader();
	return s ? s->getPass(_passIndex) : nullptr;
}

template<class V> AX_INLINE 
bool Material_Backend::setParamSpaceParam(BindSpace space, NameId name, V& v) {
	bool b = false;
	for (auto& p : _passes) {
		if (p) { b = b || p->setParamSpaceParam(space, name, v); }
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


} // namespace