module;
export module AxRender.MaterialParamSpace_Backend;
export import AxRender.ShaderParamSpace_Backend;
export import AxRender.MaterialParamSpace;
export import AxRender.GpuBuffer;
export import AxRender.Texture;

export namespace ax::AxRender {

class MaterialParamSpace_Backend : public MaterialParamSpace {
	AX_RTTI_INFO(MaterialParamSpace_Backend, MaterialParamSpace)
public:
	MaterialParamSpace_Backend(const CreateDesc& desc);

	using VarInfo	= ShaderParamSpace_Backend::VarInfo;

	struct ParamBase {
		void	  create(const ShaderParamSpace_Backend::ParamBase& shaderParam);
	protected:
	};

	struct ConstBuffer : public ParamBase {
		NameId		name() const { return _shaderParam->name(); }
		BindPoint	bindPoint() const { return _shaderParam->bindPoint(); }

		void		create(const ShaderParamSpace_Backend::ConstBuffer& shaderParam);
		GpuBuffer*	getUploadedGpuBuffer(RenderRequest* req) { return _dynamicGpuBuffer.getUploadedGpuBuffer(req); }
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

		void		   create(const ShaderParamSpace_Backend::StorageBufferParam& shaderParam);
		StorageBuffer* storageBuffer() { return _storageBuffer; }
		bool		   setStorageParam(StorageBuffer* vb) { _storageBuffer.ref(vb); return true; }
		GpuBuffer*	   gpuBuffer() { return _storageBuffer ? _storageBuffer->gpuBuffer() : nullptr; }
		Int			   dataSize() const { return _storageBuffer ? _storageBuffer->bufferSize() : 0; }

	private:
		const ShaderParamSpace_Backend::StorageBufferParam* _shaderParam = nullptr;
		SPtr<StorageBuffer>									_storageBuffer;
	};

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
	bool setParam(NameId name, Texture3D*	v);

	bool setParam(NameId name, StorageBuffer* v);

	Span<ConstBuffer>			constBuffers()	const		{ return _constBuffers;        }
	Span<TextureParam>			textureParams() const		{ return _textureParams;       }
	Span<SamplerParam>			samplerParams() const		{ return _samplerParams;       }
	Span<StorageBufferParam>	storageBufferParams() const	{ return _storageBufferParams; }

	BindSpace	bindSpace() const { return _shaderParamSpace->bindSpace(); }

	const ShaderParamSpace_Backend* shaderParamSpace() const { return _shaderParamSpace.ptr(); }

protected:
	SPtr<const ShaderParamSpace_Backend> _shaderParamSpace;

	template<class T> T* _findParam(IArray<T>& arr, NameId name);
	template<class V> bool _setVariable(NameId name, const V& v);
	template<class V> bool _setTextureParam(NameId name, V* v);

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

} // namespace
