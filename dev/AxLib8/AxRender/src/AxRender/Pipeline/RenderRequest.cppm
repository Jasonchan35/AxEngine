module;
export module AxRender:RenderRequest;
export import :RenderCommandList;

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

class RenderRequest : public RenderRequestBase {
	AX_RTTI_INFO(RenderRequest, RenderRequestBase)
public:
	using BindSpace = ShaderParamBindSpace;

	void drawMesh(   MeshObject*    mesh,    Material*	material, Int materialPass = 0, const Mat4f& objectToWorld = Mat4f::s_identity());
	void drawMesh(   RenderMesh&    mesh,    Material*	material, Int materialPass = 0, const Mat4f& objectToWorld = Mat4f::s_identity());
	void drawSubMesh(RenderSubMesh& subMesh, Material*	material, Int materialPass = 0, const Mat4f& objectToWorld = Mat4f::s_identity());

	void drawTexture(Texture2D* tex) {}

	Texture2D*		getTexture(RenderGraph_Output* v) { return nullptr; }

	Vec2i			frameSize() const { return _frameSize; }

	RenderSystem*	renderSystem() { return _renderSystem; }
	RenderContext*	renderContext() { return _renderContext; }
	RenderPass*		currentRenderPass()	{ return _currentRenderPass; }

	Int				renderRequestCount() const { return _renderRequestCount; }
	
	AX_NODISCARD	ScissorRectScope	scissorRectScope()	{ return ScissorRectScope(this); }

	void drawCall(Cmd_DrawCall& cmd);

	f64	uptime() const { return _uptime; }

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
	AX_INLINE const Rect2f& viewport() { return _viewportRect; }
	
	void setScissorRect(const Rect2f& rect);
	AX_INLINE const Rect2f& scissorRect() const { return _scissorRect; }

	RenderStockObjects* stockObjects() const { return _stockObjects; }
	
	void setCamera(const Math::Camera3f& camera);
	
protected:
	RenderSystem*  _renderSystem       = nullptr;
	RenderContext* _renderContext      = nullptr;
	RenderPass*    _currentRenderPass  = nullptr;
	Int            _renderRequestCount = 0;
	Vec2i          _frameSize{0, 0};
	Rect2f         _scissorRect{0, 0, 0, 0};
	Rect2f         _viewportRect{0, 0, 0, 0};
	f64            _uptime             = 0;
	
	RenderStockObjects*	_stockObjects = nullptr;
};

inline
ScissorRectScope::ScissorRectScope(RenderRequest* req) noexcept {
	if (!req) return;
	_rect = req->scissorRect();
	_req  = req;
}

inline
void ScissorRectScope::detach() {
	if (!_req) return;
	_req->setScissorRect(_rect);
	_req = nullptr;
}

inline
void RenderRequest::drawMesh(MeshObject* mesh, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	if (!mesh) return;
	drawMesh(mesh->meshData, material, materialPass, objectToWorld);
}

inline
void RenderRequest::drawMesh(RenderMesh& mesh, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	for (auto& sm : mesh.subMeshes()) {
		drawSubMesh(sm, material, materialPass, objectToWorld);
	}
}

inline
void RenderRequest::drawSubMesh(RenderSubMesh& subMesh, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	if (!material) return;

	Cmd_DrawCall cmd;
	cmd.material = material;
	cmd.materialPassIndex = materialPass;
	cmd.objectToWorld = objectToWorld;
	cmd.setSubMesh(this, subMesh);

	drawCall(cmd);
}


} // namespace
