module;
#include <imgui.h>
module AxRender;
import :AxImGui;
import :Renderer;
import :RenderContext;

namespace ax::AxRender {

AxImGui::~AxImGui() {
	destroy();
}

void AxImGui::create(ImFontAtlas* sharedFontAtlas) {
	destroy();

	if (!IMGUI_CHECKVERSION()) throw Error_Undefined("ImGui version error");
	_ctx = ImGui::CreateContext(sharedFontAtlas);
	if (!_ctx) throw Error_Undefined("ImGui error create context");

	_vertexBuffer.create<Vertex>();
	 _indexBuffer.create<Index>();

	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = ""; // disable auto save
	io.BackendRendererUserData = this;
	io.BackendRendererName = "AxImGui";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	io.ConfigFlags  |= ImGuiConfigFlags_NavEnableKeyboard;

	_shader   = Shader::s_new(AX_ALLOC_REQ, "ImportedAssets/Shaders/core/AxImGui.axShader");
	_material = Material::s_new(AX_ALLOC_REQ);
	_material->setShader(_shader);
}

void AxImGui::destroy() {
	if (!_ctx) return;

	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererUserData = nullptr;
	io.BackendRendererName = nullptr;

	ImGui::DestroyContext(_ctx);
	_ctx = nullptr;
}

void AxImGui::onBeginRender(Vec2i frameSize) {
	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize = ImVec2_make(frameSize);
	io.DeltaTime = 1.0f / 60.0f;

	if (!_fontTex) {
		_createFontTexture();
	}

	ImGui::NewFrame();
}

void AxImGui::onEndRender() {
}

void AxImGui::onDrawUI(RenderRequest* req) {
	ImGui::Render();

	if (!_material) return;

	auto* data = ImGui::GetDrawData();
	if (!data) return;

	if (data->DisplaySize.x <= 0 || data->DisplaySize.y <= 0)
		return;

	if (data->TotalVtxCount <= 0 || data->TotalIdxCount <= 0)
		return;

	if (req->viewportIsBottomUp()) {
		float L = data->DisplayPos.x;
		float R = data->DisplayPos.x + data->DisplaySize.x;
		float T = data->DisplayPos.y + data->DisplaySize.y;
		float B = data->DisplayPos.y;

		Mat4f projectionMatrix(
			{ 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f }
		);

		_material->setParam(AX_NAMEID("imguiProjectionMatrix"), projectionMatrix);

	} else {
		float L = data->DisplayPos.x;
		float R = data->DisplayPos.x + data->DisplaySize.x;
		float T = data->DisplayPos.y;
		float B = data->DisplayPos.y + data->DisplaySize.y;

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
	auto vertexStride = vertexLayout->stride;
	auto indexStride  = AX_SIZEOF(Index);

	AX_ASSERT(vertexStride == AX_SIZEOF(ImDrawVert));
	AX_ASSERT( indexStride == sizeof(u16));

	_vertexBuffer.ensureBufferCapacity(data->TotalVtxCount);
	 _indexBuffer.ensureBufferCapacity(data->TotalIdxCount);

	auto scissorRectScope = req->scissorRectScope();

	{
		_vertexBuffer.clear();
		_indexBuffer.clear();

		for (int i = 0; i < data->CmdListsCount; i++) {
			auto* srcCmd = data->CmdLists[i];

			for (int j = 0; j < srcCmd->CmdBuffer.Size; j++) {
				_vertexBuffer.addVertices(Span<Vertex>(reinterpret_cast<const Vertex*>(srcCmd->VtxBuffer.Data), srcCmd->VtxBuffer.Size));
				 _indexBuffer.addIndices( Span<Index >(reinterpret_cast<const Index* >(srcCmd->IdxBuffer.Data), srcCmd->IdxBuffer.Size));
			}
		}

		auto* vertexGpuBuffer = _vertexBuffer.getUploadedGpuBuffer(req);
		auto*  indexGpuBuffer =  _indexBuffer.getUploadedGpuBuffer(req);

		Int vertexStart = 0;
		Int indexStart  = 0;

		for (int i = 0; i < data->CmdListsCount; i++) {
			auto* srcCmd = data->CmdLists[i];

			for (int j = 0; j < srcCmd->CmdBuffer.Size; j++) {
				auto& srcBuf = srcCmd->CmdBuffer[j];

				// Project scissor/clipping rectangles into frame buffer space

				ImVec2 clip_off = data->DisplayPos;
				ImVec2 clip_min(srcBuf.ClipRect.x - clip_off.x, srcBuf.ClipRect.y - clip_off.y);
				ImVec2 clip_max(srcBuf.ClipRect.z - clip_off.x, srcBuf.ClipRect.w - clip_off.y);

				if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
					continue;

				// Apply scissor/clipping rectangle
				auto a = Vec2f_make(clip_min);
				auto b = Vec2f_make(clip_max);

				req->setScissorRect({a, b - a});

				Cmd_DrawCall drawcall;

				drawcall.material		   = _material;
				drawcall.materialPassIndex = 0;
				drawcall.primitiveType	   = RenderPrimitiveType::Triangles;
				drawcall.vertexLayout	   = vertexLayout;
				drawcall.vertexBuffer	   = vertexGpuBuffer;
				drawcall.vertexStart	   = vertexStart + srcBuf.VtxOffset;
				drawcall.vertexCount	   = 0;
				drawcall.indexType		   = IndexType_get<Index>;
				drawcall.indexBuffer	   = indexGpuBuffer;
				drawcall.indexCount		   = srcBuf.ElemCount;
				drawcall.indexStart		   = indexStart + srcBuf.IdxOffset;

				req->draw(drawcall);

			}

			vertexStart += srcCmd->VtxBuffer.Size;
			indexStart  += srcCmd->IdxBuffer.Size;
		}
	}
}

bool AxImGui::onUIMouseEvent(NativeUIMouseEvent& ev) {
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

void AxImGui::_createFontTexture() {
	ImGuiIO& io = ImGui::GetIO();

	unsigned char* pixels = nullptr;
	int width  = 0;
	int height = 0;
	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

	using Color = ColorRb;

	AxRender::ImageInfo	info(Color::kColorType, Vec2i(width, height));
	auto pixelData = ByteSpan(pixels, width * height);
	_fontTex = Texture2D::s_new(AX_ALLOC_REQ, info, pixelData);
}

int AxImGui::_mouseButton(NativeUIMouseEventButton v) {
	using Button = NativeUIMouseEventButton;
	switch (v) {
		case Button::Left:    return 0;
		case Button::Right:     return 1;
		case Button::Middle:    return 2;
		case Button::Button4:    return 3;
		case Button::Button5:    return 4;
		default: throw Error_Undefined();
	}
}

void AxImGui::onUIKeyEvent(NativeUIKeyEvent& ev) {
	
}

void AxImGui::setIniFilename(StrView name) {
	_iniFilename = name;
}

void AxImGui::loadIniFile() {
	if (_iniFilename) {
		ImGui::LoadIniSettingsFromDisk(_iniFilename.c_str());
	}

}

void AxImGui::saveIniFile() {
	if (_iniFilename) {
		ImGui::SaveIniSettingsToDisk(_iniFilename.c_str());
	}

}

} // namespace