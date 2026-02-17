module;

export module AxRender:ImGui_Backend;
export import :Vertex;
export import :VertexBuffer;
export import :GpuBuffer;
export import :Material;

export namespace ax /*::AxRender*/ {

class RenderContext;
class RenderRequest;

class ImGui_Backend : public NonCopyable {
public:
	ImGui_Backend();
	~ImGui_Backend();

	void create();
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
	
	static void* memAlloc(size_t size, void* user_data);
	static void  memFree(void* ptr, void* user_data);

	SPtr<Shader>	_shader;
	SPtr<Material>	_material;
	SPtr<Texture2D> _fontTex;

	VertexBuffer	_vertexBuffer;
	VertexIndexBuffer		_indexBuffer;

	String	_iniFilename;

	ImGuiContext*	_ctx = nullptr;
};

}