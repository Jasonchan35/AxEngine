module;

export module AxRender:RenderMath;
import :Common;

export namespace ax {

u32x3 ax_unpack_tri_indices(u32 packed) {
	return u32x3((packed >> 16) & 0xFF,
				 (packed >> 8 ) & 0xFF,
				 (packed      ) & 0xFF);
}
	
u32 ax_pack_tri_indices(u32 v0, u32 v1, u32 v2) {
	return (v0 << 16) | (v1 << 8) | v2;
}

u32 ax_pack_color_u32(const Color4b& v) {
	return (static_cast<u32>(v.r.v_int) << 24)
		 | (static_cast<u32>(v.g.v_int) << 16)
		 | (static_cast<u32>(v.b.v_int) << 8 )
		 | (static_cast<u32>(v.a.v_int)      );
}

u32 ax_pack_uv_u32(const Vec2f& v) {
	constexpr float kMax = 4.0f;
	auto x = static_cast<u32>(Math::clamp(v.x, 0.0f, kMax) * (65535.0f / kMax));
	auto y = static_cast<u32>(Math::clamp(v.y, 0.0f, kMax) * (65535.0f / kMax));
	return (x << 16) | y;
}

Vec2f ax_sign_not_zero(Vec2f v) {
	return Vec2f(	v.x >= 0.0f ? 1.0f : -1.0f,
					v.y >= 0.0f ? 1.0f : -1.0f);
}

Vec2f ax_octahedral_wrap(Vec2f v) { return (1.0f - v.yx().abs()) * ax_sign_not_zero(v.xy()); }

Vec2f ax_pack_normal_octahedral(Vec3f n) {
	n /= (abs(n.x) + abs(n.y) + abs(n.z));
	Vec2f t = n.z >= 0.0 ? n.xy() : ax_octahedral_wrap(n.xy());
	t = t * 0.5 + 0.5;
	return t;
}

Vec3f ax_unpack_normal_octahedral(Vec2f f) {
	using namespace Math;
	f = f * 2.0 - 1.0;
	Vec3f n = Vec3f(f.x, f.y, 1.0f - abs(f.x) - abs(f.y));
	float s = clamp01(-n.z);
	Vec2f t = n.xy() + (s * ax_sign_not_zero(n.xy()));
	n += Vec3f(t, 1);
	return n.normalize();
}


} // namespace