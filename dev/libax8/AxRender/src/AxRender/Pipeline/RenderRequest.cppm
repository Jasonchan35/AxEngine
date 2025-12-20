module;
export module AxRender:RenderRequest;
export import :CommandBuffer;

export namespace ax /*::AxRender*/ {

class RenderContext;
class RenderGraph_Output;
class RenderPassId : public NonCopyable {};

class ScissorRectScope : public NonCopyable {
public:
	ScissorRectScope() = default;
	ScissorRectScope(ScissorRectScope && r) noexcept
		: _req(r._req) , _rect(r._rect) { r._req = nullptr; }

	ScissorRectScope(RenderRequest* req) noexcept;

	~ScissorRectScope() { detach(); }

	void detach();

private:
	RenderRequest* _req = nullptr;
	Rect2f         _rect = TagZero;
};

class RenderRequest : public RenderObject {
	AX_RTTI_INFO(RenderRequest, RenderObject)
public:
	using BindSpace = ShaderParamBindSpace;
	
	RenderSeqId		renderSeqId() const		{ return _renderSeqId; }
	
	void drawMesh(   RenderMesh&    mesh,    Material*	material, Int materialPass);
	void drawSubMesh(RenderSubMesh& subMesh, Material*	material, Int materialPass);

	void drawTexture(Texture2D* tex) {}

	Texture2D*		getTexture(RenderGraph_Output* v) { return nullptr; }

	Vec2i			frameSize() const { return _frameSize; }

	Renderer*		renderer() { return _renderer; }
	RenderContext*	renderContext() { return _renderContext; }
	RenderPass*		currentRenderPass()	{ return _currentRenderPass; }

	AX_NODISCARD	ScissorRectScope	scissorRectScope()	{ return ScissorRectScope(this); }

	void drawCall(Cmd_DrawCall& cmd);
	void drawUI();

	f64	uptime() const { return _uptime; }

	bool viewportIsBottomUp() const { return _viewportIsBottomUp; }

	struct Statistics {
		bool	show = false;
		Int		drawcalls = 0;
		Int		drawTriangles = 0;
		Int		drawLines = 0;
		Int		drawPoints = 0;
		Int		vertexCount = 0;
		Int		uploadBytes = 0;
	};
	Statistics statistics;

	void setViewport(const Rect2f& rect, float minDepth, float maxDepth);
	void setScissorRect(const Rect2f& rect);
	AX_INLINE const Rect2f& scissorRect() const { return _scissorRect; }


protected:
	Renderer*      _renderer          = nullptr;
	RenderSeqId    _renderSeqId       = 0;
	RenderContext* _renderContext     = nullptr;
	RenderPass*    _currentRenderPass = nullptr;
	Vec2i          _frameSize {0,0};
	Rect2f         _scissorRect {0,0,0,0};
	Rect2f         _viewportRect {0,0,0,0};
	f64            _uptime              = 0;
	bool           _viewportIsBottomUp  = false;
};

ScissorRectScope::ScissorRectScope(RenderRequest* req) noexcept {
	if (!req) return;
	_rect = req->scissorRect();
	_req  = req;
}

void ScissorRectScope::detach() {
	if (!_req) return;
	_req->setScissorRect(_rect);
	_req = nullptr;
}

inline
void RenderRequest::drawMesh(RenderMesh& mesh, Material* material, Int materialPass) {
	for (auto& sm : mesh.subMeshes()) {
		drawSubMesh(sm, material, materialPass);
	}
}

inline
void RenderRequest::drawSubMesh(RenderSubMesh& subMesh, Material* material, Int materialPass) {
	if (!material) return;

	Cmd_DrawCall cmd;
	cmd.material = material;
	cmd.materialPassIndex = materialPass;
	cmd.setSubMesh(this, subMesh);

	drawCall(cmd);
}


} // namespace
