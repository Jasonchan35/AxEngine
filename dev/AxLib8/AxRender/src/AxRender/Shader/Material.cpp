module AxRender;
import :Material_Backend;
import :RenderSystem_Backend;

namespace ax /*::AxRender*/ {

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
	return rttiCastCheck<Material_Backend>(mtl)->setParam(ShaderParamBindSpace::Default, name, v);
}

bool Material::setParam(NameId name, const i32&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec1i32&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec2i32&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec3i32&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec4i32&	v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const u32&				v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec1u32&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec2u32&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec3u32&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec4u32&	v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const f32&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec1f&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec2f&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec3f&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec4f&	v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const f64&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec1d&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec2d&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec3d&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec4d&	v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const Color3f&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Color4f&	v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const Mat4f&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Mat4d&	v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, Sampler*		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, Texture2D*		v) { return Material_setParam(this, name, v); }

} // namespace