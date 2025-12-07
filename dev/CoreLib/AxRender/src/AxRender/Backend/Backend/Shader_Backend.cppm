module;
export module AxRender.Shader_Backend;
export import AxRender.ShaderParamSpace_Backend;
export import AxRender.ResourceHandle_Backend;
export import AxRender.Shader;

export namespace ax::AxRender {

class Shader_Backend;
class ShaderPass_Backend;

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

	bool isCompute() const { return EnumFn(_stageFlags).hasAllFlags(ShaderStageFlags::Compute); }
	
	template<class R>       R* getParamSpace_(BindSpace s)       { return rttiCastCheck<R>(getParamSpace(s)); }
	template<class R> const R* getParamSpace_(BindSpace s) const { return rttiCastCheck<R>(getParamSpace(s)); }

	ShaderParamSpace_Backend* getParamSpace(BindSpace s) {
		auto* p = _shaderParamSpaces.try_at(ax_enum_int(s));
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
		auto* pp = _passes.try_at(pass);
		auto* p  = pp ? pp->ptr() : nullptr;
		return p ? p->getParamSpace(s) : nullptr;
	}

	template<class R> const R* getPass_(Int pass) const {
		return rttiCastCheck<R>(getPass(pass));
	}

	const ShaderPass_Backend* getPass(Int pass) const {
		auto* pp = _passes.try_at(pass);
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
	if (auto* pp = _passes.try_at(i)) {
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