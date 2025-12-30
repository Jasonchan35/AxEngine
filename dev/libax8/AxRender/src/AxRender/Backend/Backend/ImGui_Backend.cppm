module;
#include <imgui.h>
export module AxRender:AxImGui;
export import :Vertex;
export import :VertexBuffer;
export import :GpuBuffer;
export import :Material;

export namespace ax /*::AxRender*/ {

class RenderContext;
class RenderRequest;

class ImGui_Backend : public NonCopyable {
public:
	~ImGui_Backend();

	void create(ImFontAtlas* sharedFontAtlas);
	void destroy();

	void onBeginRender(Vec2i frameSize);
	void onEndRender();

	void onDrawUI(RenderRequest* req);
	bool onUIMouseEvent(NativeUIMouseEvent& ev);
	void onUIKeyEvent(NativeUIKeyEvent& ev);

	void setIniFilename(StrView name);
	void loadIniFile();
	void saveIniFile();

	using Vertex = Vertex_ImGui;
	using Index  = ImDrawIdx;

private:
	void _createFontTexture();
	int _mouseButton(NativeUIMouseEventButton v);

	SPtr<Shader>	_shader;
	SPtr<Material>	_material;
	SPtr<Texture2D> _fontTex;

	VertexBuffer	_vertexBuffer;
	IndexBuffer		_indexBuffer;

	String	_iniFilename;

	ImGuiContext*	_ctx = nullptr;
};

inline Vec2f Vec2f_make(const ImVec2& v) { return Vec2f(v.x, v.y); }
inline Vec4f Vec4f_make(const ImVec4& v) { return Vec4f(v.x, v.y, v.z, v.w); }

inline ImVec2 ImVec2_make(const Vec2i& s) { return ImVec2(static_cast<float>(s.x), static_cast<float>(s.y)); }

}