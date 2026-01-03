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

bool Material::setParam(NameId name, const i32&				v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec1i32_Basic&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec2i32_Basic&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec3i32_Basic&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec4i32_Basic&	v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const u32&				v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec1u32_Basic&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec2u32_Basic&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec3u32_Basic&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec4u32_Basic&	v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const f32&				v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec1f_Basic&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec2f_Basic&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec3f_Basic&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec4f_Basic&		v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const f64&				v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec1d_Basic&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec2d_Basic&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec3d_Basic&		v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Vec4d_Basic&		v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const Color3f_Basic&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Color4f_Basic&	v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, const Mat4f_Basic&	v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, const Mat4d_Basic&	v) { return Material_setParam(this, name, v); }

bool Material::setParam(NameId name, Sampler*			v) { return Material_setParam(this, name, v); }
bool Material::setParam(NameId name, Texture2D*			v) { return Material_setParam(this, name, v); }

} // namespace