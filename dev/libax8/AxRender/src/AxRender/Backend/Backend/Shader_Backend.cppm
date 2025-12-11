module;
export module AxRender:Shader_Backend;
export import :ResourceHandle_Backend;
export import :Shader;

export namespace ax::AxRender {

class Shader_Backend;
class ShaderPass_Backend;


// mapping to DX: DescriptorTable, VK: DescriptorSet
class ShaderParamSpace_Backend : public ShaderParamSpace {
	AX_RTTI_INFO(ShaderParamSpace_Backend, ShaderParamSpace)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

	struct VarInfo {
		VarInfo() = default;
		VarInfo(const ShaderStageInfo::Variable& r)
			: _name(NameId::s_make(r.name))
			, _offset(r.offset)
			, _dataType(r.dataType)
			, _rowMajor(r.rowMajor) {}

		template<ColorModel M, class E>
		IntRange assignValueToBuffer(MutByteSpan buf, const Color_<M,E>& value) const;

		template<class V>
		IntRange assignValueToBuffer(MutByteSpan buf, const V& value) const;

		NameId	 name() const { return _name; }
		u32		 offset() const { return _offset; }
		DataType dataType() const { return _dataType; }
		bool	 rowMajor() const { return _rowMajor; }

	private:
		NameId	 _name;
		u32		 _offset	= 0;
		DataType _dataType	= DataType::None;
		bool	 _rowMajor	= true;
	};

	struct ParamBase : public NonCopyable {
		using Info = ShaderStageInfo::ParamBase;

		NameId			 name() const { return _name; }
		DataType		 dataType() const { return _dataType; }
		ShaderStageFlags stageFlags() const { return _stageFlags; }
		BindPoint		 bindPoint() const { return _bindPoint; }
		Int				 bindCount() const { return _bindCount; }

	protected:
		void	create(const Info& info);

		NameId			 _name;
		DataType		 _dataType	 = DataType::None;
		ShaderStageFlags _stageFlags = ShaderStageFlags::None;
		BindPoint		 _bindPoint	 = BindPoint::Invalid;
		Int				 _bindCount  = 0;
	};

	struct ConstBuffer : public ParamBase {
		using Info = ShaderStageInfo::ConstBuffer;

		void	create(const Info& info);

		template<class V>
		void		setVariableDefault(const VarInfo& varInfo, const V& value);
		Int			dataSize() const { return _defaultValues.size(); }
		ByteSpan	defaultValues() const { return _defaultValues; }

		Span<VarInfo>	varInfos() const { return _varInfos; }

		const VarInfo*	getVarInfo(Int i) const { return _varInfos.tryGetElement(i); }
		const VarInfo*  findVarInfo(NameId name) const {
			for (auto& v : _varInfos) {
				if (v.name() == name) return &v;
			} return nullptr;
		}

	private:
		Array<Byte>		_defaultValues;
		Array<VarInfo>	_varInfos;
	};

	struct TextureParam : public ParamBase {
		using Info = ShaderStageInfo::Texture;

		void		create(const Info& info);
		Texture*	defaultTexture() const { return ax_const_cast(_defaultTexture); }

		void setDefaultTexture(Texture* tex) {
			if (!tex || tex->type() != _dataType) { AX_ASSERT(false); return; }
			_defaultTexture = tex;
		}
	private:
		SPtr<Texture>	_defaultTexture;
	};

	struct SamplerParam : public ParamBase {
		using Info = ShaderStageInfo::Sampler;

		void		create(const Info& info);
		Sampler*	defaultSampler() const { return ax_const_cast(_defaultSampler); }
	private:
		SPtr<Sampler>	_defaultSampler;
	};

	struct StorageBufferParam : public ParamBase {
		using Info = ShaderStageInfo::StorageBuffer;

		void create(const Info& info);
	private:
	};

	Span<ConstBuffer>			constBuffers() const		{ return _constBuffers; }
	Span<TextureParam>			textureParams() const		{ return _textureParams; }
	Span<SamplerParam>			samplerParams() const		{ return _samplerParams; }
	Span<StorageBufferParam>	storageBufferParams() const	{ return _storageBufferParams; }

	void addParam	(const ShaderStageInfo::ConstBuffer&   paramInfo);
	void addParam	(const ShaderStageInfo::Texture&       paramInfo);
	void addParam	(const ShaderStageInfo::Sampler&       paramInfo);
	void addParam	(const ShaderStageInfo::StorageBuffer& paramInfo);

	ConstBuffer*		findConstBuffer (NameId name) { return _findParam(_constBuffers       , name); }
	TextureParam*		findTextureParam(NameId name) { return _findParam(_textureParams      , name); }
	SamplerParam*		findSamplerParam(NameId name) { return _findParam(_samplerParams      , name); }
	StorageBufferParam*	findStorageParam(NameId name) { return _findParam(_storageBufferParams, name); }

	const ConstBuffer*			findConstBuffer (NameId name) const { return ax_const_cast(this)->findConstBuffer(name);  }
	const TextureParam*			findTextureParam(NameId name) const { return ax_const_cast(this)->findTextureParam(name); }
	const SamplerParam*			findSamplerParam(NameId name) const { return ax_const_cast(this)->findSamplerParam(name); }
	const StorageBufferParam*	findStorageParam(NameId name) const { return ax_const_cast(this)->findStorageParam(name); }

	void setPropDefaultValue(NameId propName, const ShaderPropInfo& propInfo);

	NameId getTexture2DName(NameId name) const;
	NameId getTexture3DName(NameId name) const;
	NameId getSamplerName(NameId name) const;

protected:
	ShaderParamSpace_Backend(const CreateDesc& desc);

	template<class T, class INFO>
	void _addParam(IArray<T>& arr, const INFO& paramInfo);

	template<class T> static T* _findParam(IArray<T>& arr, NameId name);

	Array<ConstBuffer,        1>	_constBuffers;
	Array<StorageBufferParam, 0>	_storageBufferParams;
	Array<TextureParam,       4>	_textureParams;
	Array<SamplerParam,       4>	_samplerParams;

	Array<Pair<NameId, NameId>>		_nameToTexture2D;
	Array<Pair<NameId, NameId>>		_nameToTexture3D;
	Array<Pair<NameId, NameId>>		_nameToSampler;
};

template<class V> inline
IntRange ShaderParamSpace_Backend::VarInfo::assignValueToBuffer(MutByteSpan buf, const V& value) const {
	auto srcDataType = DataType_get<V>;
	if (_dataType != srcDataType)
		throw Error_Undefined(Fmt("Shader: assign variable type mismatch, from '{}' to '{}'", srcDataType, _dataType));

	IntRange range(_offset, AX_SIZEOF(value));

	if (!buf.inBound(range)) throw Error_Undefined();

	AX_GCC_WARNING_PUSH_AND_DISABLE("-Wunsafe-buffer-usage")
	auto* dst = reinterpret_cast<V*>(buf.data() + range.begin());
	AX_GCC_WARNING_POP()

	if (Math::exactlyEqual(*dst, value)) return IntRange(0, 0);

	*dst = value;
	return range;
}

template<ColorModel M, class E> AX_INLINE
IntRange ShaderParamSpace_Backend::VarInfo::assignValueToBuffer(MutByteSpan buf, const Color_<M, E>& value) const {
	return assignValueToBuffer(buf, value.toNum());
}

template<class T> AX_INLINE
T* ShaderParamSpace_Backend::_findParam(IArray<T>& arr, NameId name) {
	for (auto& e : arr) {
		if (e.name() == name) return &e;
	}
	return nullptr;
}

class ShaderPass_Backend_CreateDesc : public NonCopyable {
public:
	Shader_Backend*		   shader = nullptr;
	StrView				   name;
	const ShaderPassInfo*  info		 = nullptr;
	const ShaderStageInfo* stageInfo = nullptr;
};

class ShaderPass_Backend : public RttiObject {
	AX_RTTI_INFO(ShaderPass_Backend, RttiObject)
public:
	using CreateDesc = ShaderPass_Backend_CreateDesc;

	ShaderPass_Backend(const CreateDesc& desc);

	      Shader_Backend* shader()       { return _shader; }
	const Shader_Backend* shader() const { return _shader; }

	NameId	name() const { return _name; }

	Span<SPtr<ShaderParamSpace_Backend>>	shaderParamSpaces() const { return _shaderParamSpaces; }

	const ShaderPassInfo*	info() const	{ return _info; }

	bool isCompute() const { return EnumFn(_stageFlags).hasFlags(ShaderStageFlags::Compute); }
	
	template<class R>       R* getParamSpace_(BindSpace s)       { return rttiCastCheck<R>(getParamSpace(s)); }
	template<class R> const R* getParamSpace_(BindSpace s) const { return rttiCastCheck<R>(getParamSpace(s)); }

	ShaderParamSpace_Backend* getParamSpace(BindSpace s) {
		auto* p = _shaderParamSpaces.tryGetElement(ax_enum_int(s));
		return p ? p->ptr() : nullptr;
	}

	const ShaderParamSpace_Backend*	getParamSpace(BindSpace s) const {
		return ax_const_cast(this)->getParamSpace(s);
	}

friend class Shader_Backend;
protected:
	Shader_Backend*		_shader = nullptr;
	ShaderStageFlags	_stageFlags = ShaderStageFlags::None;
	NameId				_name;

	void _createParamSpaces();

	template<class T>
	void _addParamSpace(const Array<T>& paramInfoSpan);


	Array<SPtr<ShaderParamSpace_Backend>>	_shaderParamSpaces;

	const ShaderPassInfo*		_info = nullptr;
	const ShaderStageInfo*		_stageInfo = nullptr;
};

class Shader_Backend : public Shader {
	AX_RTTI_INFO(Shader_Backend, Shader)
public:
	ResourceHandle_Backend<This>	resourceHandle;

	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView assetPath);

	virtual UPtr<ShaderPass_Backend > onNewPass (const ShaderPass_Backend_CreateDesc& desc) = 0;

	const ShaderResultInfo* info() const { return &_info; }
	MutSpan<UPtr<ShaderPass_Backend>>	passes() { return _passes; }

	ShaderPass_Backend*		getPass(Int i);
	const ShaderPropInfo*	findPropInfo(NameId name) const;

	NameId getPropSamplerName(NameId name) const;

	template<class R> const R* getPassParamSpace_(Int pass, BindSpace s) const {
		return rttiCastCheck<R>(getPassParamSpace(pass, s));
	}

	const ShaderParamSpace_Backend*	getPassParamSpace(Int pass, BindSpace s) const {
		auto* pp = _passes.tryGetElement(pass);
		auto* p  = pp ? pp->ptr() : nullptr;
		return p ? p->getParamSpace(s) : nullptr;
	}

	template<class R> const R* getPass_(Int pass) const {
		return rttiCastCheck<R>(getPass(pass));
	}

	const ShaderPass_Backend* getPass(Int pass) const {
		auto* pp = _passes.tryGetElement(pass);
		return pp ? pp->ptr() : nullptr;
	}

	bool isGlobalCommonShader() const { return _isGlobalCommonShader; }

	void hotReloadFile();

protected:
	Shader_Backend(const CreateDesc& desc);

	void _create(const CreateDesc& desc) { onCreate(desc); }
	virtual void onCreate(const CreateDesc& desc) { onLoadFile(); }
	virtual void onLoadFile();

	void destroy() { onDestroy(); }
	virtual void onDestroy();

	bool	_isGlobalCommonShader = false;

	ShaderResultInfo	_info;

	struct PropName {
		NameId name;
		NameId samplerName;
		const ShaderPropInfo* prop = nullptr;
	};

	Array<PropName, 8>	_propNameIds;
	Array<UPtr<ShaderPass_Backend>> _passes;
};

inline ShaderPass_Backend* Shader_Backend::getPass(Int i) {
	if (auto* pp = _passes.tryGetElement(i)) {
		return pp->ptr();
	}
	return nullptr;
}

inline
const ShaderPropInfo* Shader_Backend::findPropInfo(NameId name) const {
	for (auto& p : _propNameIds) {
		if (p.name == name) return p.prop;
	}
	return nullptr;
}

inline
NameId Shader_Backend::getPropSamplerName(NameId name) const {
	for (auto& p : _propNameIds) {
		if (p.name == name) return p.samplerName;
	}
	return NameId();
}


} // namespace