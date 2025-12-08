module;
export module AxRender:ShaderParamSpace_Backend;
export import :ShaderParamSpace;
export import :ShaderInfo;

export namespace ax::AxRender {

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

		const VarInfo*	getVarInfo(Int i) const { return _varInfos.try_at(i); }
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

	AX_PRAGMA_GCC(diagnostic push)
	AX_PRAGMA_GCC(diagnostic ignored "-Wunsafe-buffer-usage")
	auto* dst = reinterpret_cast<V*>(buf.data() + range.begin());
	AX_PRAGMA_GCC(diagnostic pop)

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

} // namespace
