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


} // namespace