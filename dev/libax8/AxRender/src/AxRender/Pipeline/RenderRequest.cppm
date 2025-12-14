module;
export module AxRender:RenderRequest;
export import :CommandBuffer;

export namespace ax::AxRender {

class RenderContext;
class RenderGraph_Output;
class RenderPassId : public NonCopyable {};

class ScissorRectScope : public NonCopyable {
public:
	ScissorRectScope() = default;

	ScissorRectScope(ScissorRectScope && r) noexcept {
		_cmdBuf = r._cmdBuf;
		_rect = r._rect;
		r._cmdBuf = nullptr;
	}

	ScissorRectScope(CommandBuffer* cmdBuf) noexcept {
		if (!cmdBuf) return;
		_rect	= cmdBuf->scissorRect();
		_cmdBuf = cmdBuf;
	}

	~ScissorRectScope() { detach(); }

	void detach() {
		if (!_cmdBuf) return;
		_cmdBuf->setScissorRect(_rect);
		_cmdBuf = nullptr;
	}

private:
	CommandBuffer* _cmdBuf = nullptr;
	Rect2f	_rect;
};

class RenderRequest : public RttiObject {
	AX_RTTI_INFO(RenderRequest, RttiObject)
public:
	using ParamSpaceType = ShaderParamSpaceType;
	
	RenderSeqId		renderSeqId() const		{ return _renderSeqId; }

	void setViewport(const Rect2f& rect, float minDepth, float maxDepth) { _graphCmdBuf->setViewport(rect, minDepth, maxDepth); }
	
	void draw(Cmd_DrawCall& cmd) { onDrawcall(cmd); }
	
	void drawMesh(   RenderMesh&    mesh,    Material*	material, Int materialPass);
	void drawSubMesh(RenderSubMesh& subMesh, Material*	material, Int materialPass);

	void drawTexture(Texture2D* tex) {}

	Texture2D* getTexture(RenderGraph_Output* v) { return nullptr; }

	Vec2i			frameSize() const { return _frameSize; }

	Renderer*		renderer() { return _renderer; }
	RenderContext*	renderContext() { return _renderContext; }
	RenderPass*		currentRenderPass()	{ return _currentRenderPass; }

	AX_NODISCARD	ScissorRectScope	scissorRectScope()	{ return ScissorRectScope(_graphCmdBuf); }

	AX_INLINE		const Rect2f& scissorRect() const { return _graphCmdBuf->scissorRect(); }
	AX_INLINE		void setScissorRect(const Rect2f& rect) { _graphCmdBuf->setScissorRect(rect); }

	void drawUI() { onDrawUI(); }

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

protected:
	virtual void onDrawcall(Cmd_DrawCall& cmd) = 0;
	virtual void onDrawUI() = 0;

	Renderer*			_renderer = nullptr;
	RenderSeqId			_renderSeqId = 0;
	RenderContext*		_renderContext = nullptr;
	RenderPass*			_currentRenderPass = nullptr;
	Vec2i				_frameSize {0,0};
	f64					_uptime = 0;
	CommandBuffer*		_graphCmdBuf = nullptr;

	bool	_viewportIsBottomUp = false;
};

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

	draw(cmd);
}


} // namespace
