module;
export module AxRender:Shader_Backend;
export import :ResourceHandle_Backend;
export import :Shader;

export namespace ax /*::AxRender*/ {

class Shader_Backend;
class ShaderPass_Backend;

class ShaderParamSpace_CreateDesc : public NonCopyable {
public:
	using BindSpace = ShaderParamBindSpace;
	ShaderPass_Backend* shaderPass = nullptr;
	BindSpace bindSpace = BindSpace::Invalid;
};

// mapping to DX: DescriptorTable, VK: DescriptorSet
class ShaderParamSpace_Backend : public RenderObject {
	AX_RTTI_INFO(ShaderParamSpace_Backend, RenderObject)
public:
	using CreateDesc = ShaderParamSpace_CreateDesc;
	using BindPoint  = ShaderParamBindPoint;
	using BindSpace  = ShaderParamBindSpace;
	using BindCount  = ShaderParamBindCount;
	
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
		RenderDataType dataType() const { return _dataType; }
		bool	 rowMajor() const { return _rowMajor; }

	private:
		NameId         _name;
		u32            _offset   = 0;
		RenderDataType _dataType = RenderDataType::None;
		bool           _rowMajor = true;
	};

	using ParamIndex = u32;
	
	struct ParamBase : public NonCopyable {
		using Info = ShaderStageInfo::ParamBase;

		NameId           name() const		{ return _name; }
		RenderDataType   dataType() const	{ return _dataType; }
		ShaderStageFlags stageFlags() const { return _stageFlags; }
		BindPoint        bindPoint() const	{ return _bindPoint; }
		BindCount        bindCount() const	{ return _bindCount; }

	protected:
		void	create(const Info& info);

		NameId               _name;
		RenderDataType       _dataType   = RenderDataType::None;
		ShaderParamBindPoint _bindPoint  = ShaderParamBindPoint::Invalid;
		ShaderParamBindCount _bindCount  = 0;
		ShaderStageFlags     _stageFlags = ShaderStageFlags::None;
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

		void setDefaultSampler(Sampler* sampler) {
			_defaultSampler = sampler;
		}
	private:
		SPtr<Sampler>	_defaultSampler;
	};

	struct StorageBufferParam : public ParamBase {
		using Info = ShaderStageInfo::StorageBuffer;

		void create(const Info& info);
	private:
	};

	void addParam	(const ShaderStageInfo::ConstBuffer&   paramInfo);
	void addParam	(const ShaderStageInfo::Texture&       paramInfo);
	void addParam	(const ShaderStageInfo::Sampler&       paramInfo);
	void addParam	(const ShaderStageInfo::StorageBuffer& paramInfo);

	Array<ConstBuffer,        1>	_constBuffers;
	Array<StorageBufferParam, 0>	_storageBufferParams;
	Array<TextureParam,       4>	_textureParams;
	Array<SamplerParam,       4>	_samplerParams;

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

	Int totalBindCount_constBuffers        () const	{ return _totalBindCount_constBuffers        ; }
	Int totalBindCount_textureParams       () const	{ return _totalBindCount_textureParams       ; }
	Int totalBindCount_samplerParams       () const	{ return _totalBindCount_samplerParams       ; }
	Int totalBindCount_storageBufferParams () const	{ return _totalBindCount_storageBufferParams ; }

	BindSpace bindSpace() const { return _bindSpace; }
	
protected:
	friend class ShaderPass_Backend;

	ShaderParamSpace_Backend(const CreateDesc& desc);
	void _postCreate(ShaderPass_Backend* shdPass);

	template<class T, class INFO>
	void _addParam(IArray<T>& arr, const INFO& paramInfo);

	template<class T> static T* _findParam(IArray<T>& arr, NameId name);

	ShaderPass_Backend* _shaderPass = nullptr;
	BindSpace _bindSpace = BindSpace::Invalid;
	
	Int _totalBindCount_constBuffers        = 0;
	Int _totalBindCount_textureParams       = 0;
	Int _totalBindCount_samplerParams       = 0;
	Int _totalBindCount_storageBufferParams = 0;
	
	Array<Pair<NameId, NameId>>		_nameToTexture2D;
	Array<Pair<NameId, NameId>>		_nameToTexture3D;
	Array<Pair<NameId, NameId>>		_nameToSampler;
};

template<class V> inline
IntRange ShaderParamSpace_Backend::VarInfo::assignValueToBuffer(MutByteSpan buf, const V& value) const {
	auto srcDataType = DataType_get<V>;
	if (_dataType != srcDataType)
		throw Error_Undefined(Fmt("Shader: assign variable type mismatch, from '{}' to '{}'", srcDataType, _dataType));

	IntRange range = IntRange_StartAndSize(_offset, AX_SIZEOF(value));

	if (!buf.inBound(range)) throw Error_Undefined();

	AX_GCC_WARNING_PUSH_AND_DISABLE("-Wunsafe-buffer-usage")
	auto* dst = reinterpret_cast<V*>(buf.data() + range.start());
	AX_GCC_WARNING_POP()

	if (Math::exactlyEqual(*dst, value)) return IntRange();

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

class ShaderPass_CreateDesc : public NonCopyable {
public:
	Shader_Backend*        shader    = nullptr;
	Int                    passIndex = 0;
	StrView                name;
	const ShaderPassInfo*  info      = nullptr;
	const ShaderStageInfo* stageInfo = nullptr;
};

class ShaderPass_Backend : public RenderObject {
	AX_RTTI_INFO(ShaderPass_Backend, RenderObject)
public:
	using CreateDesc = ShaderPass_CreateDesc;
	using ParamBase = ShaderParamSpace_Backend::ParamBase;
	using BindPoint = ShaderParamBindPoint;
	using BindCount = ShaderParamBindCount;
	using BindSpace = ShaderParamBindSpace;
	static constexpr auto BindSpace_COUNT = ax_enum_int(BindSpace::_COUNT);

	ShaderPass_Backend(const CreateDesc& desc);

	      Shader_Backend* shader()       { return _shader; }
	const Shader_Backend* shader() const { return _shader; }

	const ShaderPassInfo*	info() const	{ return _info; }
	bool isGlobalCommonShaderPass() const	{ return _isGlobalCommonShaderPass; }

	bool isOwnParamSpace(BindSpace s) const {
		return _isGlobalCommonShaderPass || s == BindSpace::Default || s == BindSpace::Object;
	}

	bool isCompute() const { return ax_bit_has(_stageFlags, ShaderStageFlags::Compute); }

	const ShaderParamSpace_Backend* getParamSpace(BindSpace bs) const {
		auto* p = _shaderParamSpaces.tryGetElement(ax_enum_int(bs));
		return p ? p->ptr() : nullptr;
	}

	ShaderParamSpace_Backend* getOwnParamSpace(BindSpace bs) {
		if (!isOwnParamSpace(bs)) return nullptr;
		return ax_const_cast(_shaderParamSpaces[ax_enum_int(bs)].ptr());
	}

	Int ownParamSpaceTotalBindCount_constBuffers       () const { return _ownParamSpaceTotalBindCount_constBuffers       ; }
	Int ownParamSpaceTotalBindCount_textureParams      () const { return _ownParamSpaceTotalBindCount_textureParams      ; }
	Int ownParamSpaceTotalBindCount_samplerParams      () const { return _ownParamSpaceTotalBindCount_samplerParams      ; }
	Int ownParamSpaceTotalBindCount_storageBufferParams() const { return _ownParamSpaceTotalBindCount_storageBufferParams; }
	
private:
	FixedArray<SPtr<const ShaderParamSpace_Backend>, BindSpace_COUNT>	_shaderParamSpaces;
	
protected:
	friend class Shader_Backend;
	
	Shader_Backend*		_shader = nullptr;
	bool				_isGlobalCommonShaderPass : 1 = false;
	Int					_passIndex = 0;
	ShaderStageFlags	_stageFlags = ShaderStageFlags::None;
	NameId				_name;

	Int _ownParamSpaceTotalBindCount_constBuffers        = 0;
	Int _ownParamSpaceTotalBindCount_textureParams       = 0;
	Int _ownParamSpaceTotalBindCount_samplerParams       = 0;
	Int _ownParamSpaceTotalBindCount_storageBufferParams = 0;

	void _createParamSpaces();

	template<class T>
	void _addParamToSpace(const Array<T>& paramInfoSpan);
	
	const ShaderPassInfo*		_info = nullptr;
	const ShaderStageInfo*		_stageInfo = nullptr;
};

class Shader_Backend : public Shader {
	AX_RTTI_INFO(Shader_Backend, Shader)
public:
	ResourceHandle_Backend<This>	resourceHandle;

	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView assetPath);

	virtual UPtr<ShaderPass_Backend > onNewPass (const ShaderPass_CreateDesc& desc) = 0;

	const ShaderResultInfo* info() const { return &_info; }
	MutSpan<UPtr<ShaderPass_Backend>>	passes() { return _passes; }

	Int passCount() const { return _passes.size(); }
	ShaderPass_Backend*		getPass(Int i);
	const ShaderPropInfo*	findPropInfo(NameId name) const;

	NameId getPropSamplerName(NameId name) const;

	const ShaderParamSpace_Backend*	getPassParamSpace(Int pass, BindSpace s) const {
		auto* pp = _passes.tryGetElement(pass);
		auto* p  = pp ? pp->ptr() : nullptr;
		return p ? p->getParamSpace(s) : nullptr;
	}

	const ShaderPass_Backend* getPass(Int pass) const {
		auto* pp = _passes.tryGetElement(pass);
		return pp ? pp->ptr() : nullptr;
	}
	
	AX_INLINE bool isGlobalCommonShader() const { return _isGlobalCommonShader; }

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