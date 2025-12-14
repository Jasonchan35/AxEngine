module AxRender;
import :Material_Backend;
import :Renderer_Backend;

namespace ax::AxRender {

SPtr<Material> Material::s_new(const MemAllocRequest& req, Shader* shader) {
	return Material_Backend::s_new(req, shader);
}

SPtr<Material> Material::s_new(const MemAllocRequest& req, StrView shaderAssetPath) {
	return Material_Backend::s_new(req, shaderAssetPath);
}

void Material::setShader(Shader* shader) {
	rttiCastCheck<Material_Backend>(this)->setShader_backend(shader);
}

Shader* Material::shader() {
	return rttiCastCheck<Material_Backend>(this)->shader_backend();
}

Material::Material(const CreateDesc& desc) {
}

template<class V> AX_INLINE
bool Material_setParam(Material* mtl, NameId name, const V& v) {
	return rttiCastCheck<Material_Backend>(mtl)->setParamSpaceParam(BindSpace::Default, name, v);
}

bool Material::setParam(NameId name, const i32&			v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const i32x2&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const i32x3&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const i32x4&		v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const u32&			v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const u32x2&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const u32x3&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const u32x4&		v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const f32&			v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const f32x2&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const f32x3&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const f32x4&		v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const f64&			v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const f64x2&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const f64x3&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const f64x4&		v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const Color3f&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Color4f&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Mat4f&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Mat4d&		v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, Sampler*			v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, Texture2D*			v) { return Material_setParam(this, name, v); }

} // namespace