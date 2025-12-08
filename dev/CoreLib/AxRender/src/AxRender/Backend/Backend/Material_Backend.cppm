module;
export module AxRender.Material_Backend;
export import AxRender.Shader_Backend;
export import AxRender.Texture_Backend;
export import AxRender.MaterialParamSpace_Backend;
export import AxRender.Material;

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

class Cmd_DrawCall;
class Material_Backend;
class MaterialPass_Backend;

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
		auto* pp = _materialParamSpaces.try_at(ax_enum_int(s));
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

	void setShader(Shader* shader);
	Shader_Backend* shader() { return _shader; }

	template<class V> AX_INLINE
	bool setParamSpaceParam(BindSpace space, NameId name, V& v);

	Int		passCount() const { return _passes.size(); }

	MaterialPass_Backend*	getPass(Int index);

	StrView shaderAssetPath() const { return _shader ? StrView(_shader->assetPath()) : StrView(); }

	template<class R> R* getPassParamSpace_(Int pass, BindSpace s) {
		return rttiCastCheck<R>(getPassParamSpace(pass, s));
	}

	MaterialParamSpace_Backend*	getPassParamSpace(Int passIndex, BindSpace s) {
		auto* pp = _passes.tryGet(passIndex);
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
	return m ? m->shader() : nullptr;
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