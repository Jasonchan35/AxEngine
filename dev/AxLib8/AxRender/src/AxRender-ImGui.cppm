module;

export module AxRender.ImGui;
import AxCore;

export namespace ax {

inline Vec2f Vec2f_make(const ImVec2& v) { return Vec2f(v.x, v.y); }
inline Vec4f Vec4f_make(const ImVec4& v) { return Vec4f(v.x, v.y, v.z, v.w); }

inline ImVec2 ImVec2_make(const Vec2f& s) { return ImVec2(s.x, s.y); }
inline ImVec4 ImVec4_make(const Vec4f& s) { return ImVec4(s.x, s.y, s.z, s.w); }

} // namespace