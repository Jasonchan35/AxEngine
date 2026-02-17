module;

module AxRender;
import :ImGui_Backend;
import :RenderSystem;
import :RenderContext;
import AxRender.ImGui;

namespace ax /*::AxRender*/ {

ImGui_Backend::ImGui_Backend() {

}

ImGui_Backend::~ImGui_Backend() {
	destroy();
}

void ImGui_Backend::create() {
	destroy();

	ImGui::SetAllocatorFunctions(memAlloc, memFree, this);
	
	if (!IMGUI_CHECKVERSION()) throw Error_Undefined("ImGui version error");
	
	_ctx = ImGui::CreateContext();
	if (!_ctx) throw Error_Undefined("ImGui error create context");

	_vertexBuffer.create<Vertex>();
	 _indexBuffer.create<Index>();

	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = ""; // disable auto save
	io.BackendRendererUserData = this;
	io.BackendRendererName = "AxImGui";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	io.ConfigFlags  |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags  |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags  |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	
	constexpr float uiScale  = 1.25f;
	
	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(uiScale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = uiScale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
	io.ConfigDpiScaleFonts = false;      // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
	io.ConfigDpiScaleViewports = false;  // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
	
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	_shader   = Shader::s_new(AX_NEW, "ImportedAssets/Shaders/core/AxImGui.axShader");
	_material = Material::s_new(AX_NEW);
	_material->setShader(_shader);
}

void ImGui_Backend::destroy() {
	if (!_ctx) return;

	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererUserData = nullptr;
	io.BackendRendererName = nullptr;

	ImGui::DestroyContext(_ctx);
	_ctx = nullptr;
}

void ImGui_Backend::onBeginRender(Vec2i frameSize) {
	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize = ImVec2_make(Vec2f::s_cast(frameSize));
	io.DeltaTime = 1.0f / 60.0f;

	if (!_fontTex) {
		_createFontTexture();
	}

	ImGui::NewFrame();
	
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::BeginFrame();
}

void ImGui_Backend::onEndRender() {
}

void ImGui_Backend::onDrawUI(RenderRequest* req) {
	static bool show_demo_window;
	ImGui::ShowDemoWindow(&show_demo_window);
	
	ImGui::Render();

	if (!_material) return;

	auto* data = ImGui::GetDrawData();
	if (!data) return;

	if (data->DisplaySize.x <= 0 || data->DisplaySize.y <= 0)
		return;

	if (data->TotalVtxCount <= 0 || data->TotalIdxCount <= 0)
		return;

	{
		float L = data->DisplayPos.x;
		float R = data->DisplayPos.x + data->DisplaySize.x;
		float T = data->DisplayPos.y;
		float B = data->DisplayPos.y;

	#if 1
		B += data->DisplaySize.y;
	#else
		if (req->viewportIsBottomUp()) {
			T += data->DisplaySize.y;
		} else {
			B += data->DisplaySize.y;
		}
	#endif

		Mat4f projectionMatrix(
			{ 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f }
		);
		_material->setParam(AX_NAMEID("imguiProjectionMatrix"), projectionMatrix);
	}

	_material->setParam(AX_NAMEID("fontTexture"), _fontTex);

	auto vertexLayout = _vertexBuffer.vertexLayout();
	auto vertexStride = vertexLayout->strideInBytes;
	auto indexStride  = AX_SIZEOF(Index);

	AX_ASSERT(vertexStride == AX_SIZEOF(ImDrawVert));
	AX_ASSERT( indexStride == sizeof(u16));

	// static constexpr auto kVertexSize = AX_SIZEOF(ImDrawVert);
	// static constexpr auto kIndexSize  = AX_SIZEOF(ImDrawIdx);
	
	_vertexBuffer.ensureBufferCapacity(data->TotalVtxCount);
	_indexBuffer.ensureBufferCapacity(data->TotalIdxCount);

	auto scissorRectScope = req->scissorRectScope();

	_vertexBuffer.clear();
	_indexBuffer.clear();

	for (int i = 0; i < data->CmdListsCount; i++) {
		auto* srcCmd = data->CmdLists[i];
		auto* pVtx = reinterpret_cast<const Vertex*>(srcCmd->VtxBuffer.Data);
		_vertexBuffer.addVertices(Span<Vertex>(pVtx, srcCmd->VtxBuffer.Size));
		
		auto* pIdx = reinterpret_cast<const Index* >(srcCmd->IdxBuffer.Data);
		_indexBuffer.addIndices(  Span<Index >(pIdx, srcCmd->IdxBuffer.Size));
	}

	auto* vertexGpuBuffer = _vertexBuffer.getUploadedGpuBuffer(req);
	auto*  indexGpuBuffer =  _indexBuffer.getUploadedGpuBuffer(req);

	Int vertexStart = 0;
	Int indexStart  = 0;

	for (int i = 0; i < data->CmdListsCount; i++) {
		auto* srcCmd = data->CmdLists[i];

		for (int j = 0; j < srcCmd->CmdBuffer.Size; j++) {
			auto& srcCmdBuf = srcCmd->CmdBuffer[j];

			// Project scissor/clipping rectangles into frame buffer space

			ImVec2 clip_off = data->DisplayPos;
			ImVec2 clip_min(srcCmdBuf.ClipRect.x - clip_off.x, srcCmdBuf.ClipRect.y - clip_off.y);
			ImVec2 clip_max(srcCmdBuf.ClipRect.z - clip_off.x, srcCmdBuf.ClipRect.w - clip_off.y);

			if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
				continue;

			// Apply to scissor/clipping rectangle
			auto a = Vec2f_make(clip_min);
			auto b = Vec2f_make(clip_max);

			req->setScissorRect({a, b - a});

			AxVertexShaderDraw draw;

			draw.material		   = _material;
			draw.materialPassIndex = 0;
			draw.primitiveType	   = RenderPrimitiveType::Triangles;
			draw.vertexLayout	   = vertexLayout;
			draw.vertexBuffer	   = vertexGpuBuffer;
			draw.vertexStart	   = vertexStart + srcCmdBuf.VtxOffset;
			draw.vertexCount	   = 0;
			draw.indexType		   = VertexIndexType_get<Index>;
			draw.indexBuffer	   = indexGpuBuffer;
			draw.indexCount		   = srcCmdBuf.ElemCount;
			draw.indexStart		   = indexStart + srcCmdBuf.IdxOffset;

			req->vertexShaderDraw(draw);
		}

		vertexStart += srcCmd->VtxBuffer.Size;
		indexStart  += srcCmd->IdxBuffer.Size;
	}

}

bool ImGui_Backend::onUIMouseEvent(NativeUIMouseEvent& ev) {
	ImGuiIO& io = ImGui::GetIO();

	using Type = NativeUIMouseEventType;
	switch (ev.type) {
		case Type::Move: {
			io.AddMousePosEvent(ev.pos.x, ev.pos.y);
		} break;

		case Type::Down: {
			io.AddMouseButtonEvent(_mouseButton(ev.button), true);
		} break;

		case Type::Up: {
			io.AddMouseButtonEvent(_mouseButton(ev.button), false);
		} break;

		case Type::Wheel: {
			io.AddMouseWheelEvent(ev.wheelDelta.x, ev.wheelDelta.y);
		} break;

		default: break;
	}

	return io.WantCaptureMouse;
}

void ImGui_Backend::_createFontTexture() {
	ImGuiIO& io = ImGui::GetIO();

	unsigned char* pixels = nullptr;
	int width  = 0;
	int height = 0;
	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

	using Color = ColorRb;

	ImageInfo	info(Color::kColorType, Vec2i(width, height));
	auto pixelData = ByteSpan(pixels, width * height);
	_fontTex = Texture2D::s_new(AX_NEW, info, pixelData);
}

int ImGui_Backend::_mouseButton(NativeUIMouseEventButton v) {
	using Button = NativeUIMouseEventButton;
	switch (v) {
		case Button::Left:    return 0;
		case Button::Right:   return 1;
		case Button::Middle:  return 2;
		case Button::Button4: return 3;
		case Button::Button5: return 4;
		default: throw Error_Undefined();
	}
}

void* ImGui_Backend::memAlloc(size_t size, void* user_data) {
	MemAllocRequest req(ax_current_allocator());
	req.dataSize = size;
	auto result = ax_current_allocator()->allocBytes(req);
	return result.takeOwnership();
}

void ImGui_Backend::memFree(void* ptr, void* user_data) {
	ax_current_allocator()->dealloc(ptr);
}

void ImGui_Backend::onUIKeyEvent(NativeUIKeyEvent& ev) {
	
}

void ImGui_Backend::setIniFilename(StrView name) {
	_iniFilename = name;
}

void ImGui_Backend::loadIniFile() {
	if (_iniFilename) {
		ImGui::LoadIniSettingsFromDisk(_iniFilename.c_str());
	}

}

void ImGui_Backend::saveIniFile() {
	if (_iniFilename) {
		ImGui::SaveIniSettingsToDisk(_iniFilename.c_str());
	}

}

} // namespace