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

class RenderRequest : public RenderRequestBase {
	AX_RTTI_INFO(RenderRequest, RenderRequestBase)
public:
	using BindSpace = ShaderParamBindSpace;

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
		bool show                     = false;
		Int  drawcalls                = 0;
		Int  drawTriangles            = 0;
		Int  drawLines                = 0;
		Int  drawPoints               = 0;
		Int  vertexCount              = 0;
		Int  uploadBytes              = 0;
		
		GpuBufferPool::Statistics  meshObject;
		GpuBufferPool::Statistics  meshletVert;
		GpuBufferPool::Statistics  meshletPrim;
		GpuBufferPool::Statistics  meshletCluster;
		GpuBufferPool::Statistics  meshletGroup;
	};
	Statistics statistics;

	void setViewport(const Rect2f& rect, float minDepth, float maxDepth);
	AX_INLINE const Rect2f& viewport() { return _viewportRect; }
	
	void setScissorRect(const Rect2f& rect);
	AX_INLINE const Rect2f& scissorRect() const { return _scissorRect; }

	RenderStockObjects* stockObjects() const { return _stockObjects; }
	
	const AxGpuData_World& world() { return _worldData; }
	
	void setCamera(const Math::Camera3f& camera);
	const AxGpuData_Camera&	cameraData() const { return _cameraData; }

	void setDebugData(const AxGpuData_Debug& debugData);
	
	float maxMeshletErrorInPixels = 1;
	
protected:
	RenderRequest();
	
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

	AxGpuData_World  _worldData;
	AxGpuData_Camera _cameraData;
	AxGpuData_Debug  _debugData;
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
