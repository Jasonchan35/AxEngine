module;
export module AxRender:RenderRequest;
export import :RenderCommandList;
export import :MeshObject;

export namespace ax /*::AxRender*/ {

class RenderContext;
class RenderGraph;
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

struct AxRenderGpuData_Camera {
	Vec3f	worldPos;
	Vec2f	viewportMin;
	Vec2f	viewportMax;
	Mat4f	projMatrix;
	Mat4f	projMatrixInv;
	Mat4f	viewMatrix;
	Mat4f	viewMatrixInv;
	Mat4f	viewProjMatrix;
	Mat4f	viewProjMatrixInv;
};

class RenderRequest : public RenderRequestBase {
	AX_RTTI_INFO(RenderRequest, RenderRequestBase)
public:
	using BindSpace = ShaderParamBindSpace;

	void drawMeshRenderer(MeshObjectRenderer* mr);
	void drawMesh(MeshObject*    mesh,    Material*	material, Int materialPass = 0, const Mat4f& objectToWorld = Mat4f::s_identity());
	void drawMesh(RenderMesh&    mesh,    Material*	material, Int materialPass = 0, const Mat4f& objectToWorld = Mat4f::s_identity());
	void drawSubMesh(RenderSubMesh& subMesh, Material*	material, Int materialPass = 0, const Mat4f& objectToWorld = Mat4f::s_identity());

	void drawWorld();
	
	void drawTexture(Texture2D* tex) {}

	Texture2D*		getTexture(RenderGraph_Output* v) { return nullptr; }

	Vec2i			frameSize() const { return _frameSize; }

	RenderSystem*	renderSystem() { return _renderSystem; }
	RenderContext*	renderContext() { return _renderContext; }
	RenderPass*		currentRenderPass()	{ return _currentRenderPass; }

	AX_NODISCARD	ScissorRectScope	scissorRectScope()	{ return ScissorRectScope(this); }

	void vertexShaderDraw(AxVertexShaderDraw& draw);
	void meshShaderDraw(AxMeshShaderDraw& draw);

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
	const AxRenderGpuData_Camera&	cameraData() const { return _cameraData; }

protected:
	RenderSystem*       _renderSystem       = nullptr;
	RenderContext*      _renderContext      = nullptr;
	const RenderGraph*  _renderGraph        = nullptr;
	RenderStockObjects* _stockObjects       = nullptr;
	RenderPass*         _currentRenderPass  = nullptr;
	ProjectionDesc      _projectionDesc;
	f64                 _uptime             = 0;
	Vec2i               _frameSize{0, 0};
	Rect2f              _scissorRect{0, 0, 0, 0};
	Rect2f              _viewportRect{0, 0, 0, 0};

	AxRenderGpuData_Camera _cameraData;
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

} // namespace
