module;
export module AxRender:CommandBuffer;
export import :DataType;
export import :VertexLayout;
export import :GpuBuffer;
export import :Material;
export import :RenderBuffer;
export import :RenderPass;
export import :RenderMesh;

export namespace ax::AxRender {

class Cmd_DrawCall : public NonCopyable {
public:
	PrimitiveType	primitiveType	= PrimitiveType::Triangles;
	IndexType		indexType		= IndexType::None;

	VertexLayout	vertexLayout = nullptr;

	GpuBuffer*		vertexBuffer	= nullptr;
	Int				vertexStart		= 0;
	Int				vertexCount		= 0;

	GpuBuffer*		indexBuffer		= nullptr;
	Int				indexStart		= 0;
	Int				indexCount		= 0;

	Int				instanceStart	= 0;
	Int				instanceCount	= 1;

	Material*		material		= nullptr;
	Int				materialPassIndex = 0;

	void setSubMesh(class RenderRequest* req, RenderSubMesh& sm) {
		primitiveType = sm.primitiveType();
		setVertexBuffer(req, sm.vertexBuffer);
		setIndexBuffer( req, sm.indexBuffer);
	}

	void setVertexBuffer(class RenderRequest* req, VertexBuffer& vb) {
		vertexLayout = vb.vertexLayout();
		vertexCount  = vb.vertexCount();
		vertexBuffer = vb.getUploadedGpuBuffer(req);
	}

	void setIndexBuffer(class RenderRequest* req, IndexBuffer& ib) {
		indexType    = ib.indexType();
		indexCount   = ib.indexCount();
		indexBuffer  = ib.getUploadedGpuBuffer(req);
	}

	void setMaterialPass(class Material* material_, Int passIndex) {
		material = material_;
		materialPassIndex = passIndex;
	}
};

#define AX_RENDER_CommandBuffer_FunctionInterfaces(IMP) \
	virtual void onCommandBegin() IMP; \
	virtual void onCommandEnd() IMP; \
	virtual void onSetViewport(const Rect2f& rect, float minDepth, float maxDepth) IMP; \
	virtual void onSetScissorRect(const Rect2f& rect) IMP; \
	virtual void onRenderPassBegin(RenderPass* pass) IMP; \
	virtual void onRenderPassEnd() IMP; \
	virtual void onDrawCall(Cmd_DrawCall& cmd) IMP; \
//-----

class CommandBuffer : public RenderObject {
	AX_RTTI_INFO(CommandBuffer, RenderObject)
public:
	AX_INLINE void commandBegin() { onCommandBegin(); }
	AX_INLINE void commandEnd() { onCommandEnd(); }
	AX_INLINE void setViewport(const Rect2f& rect, float minDepth, float maxDepth) {
		_scissorRect = rect;
		onSetViewport(rect, minDepth, maxDepth);
		onSetScissorRect(rect);
	}

	AX_INLINE const Rect2f& scissorRect() const { return _scissorRect; }

	AX_INLINE void setScissorRect(const Rect2f& rect) {
		if (Math::exactlyEqual(_scissorRect, rect)) return;
		_scissorRect = rect;
		onSetScissorRect(rect);
	}

	AX_INLINE void renderPassBegin(RenderPass* pass) { onRenderPassBegin(pass); }
	AX_INLINE void renderPassEnd() { onRenderPassEnd(); }
	AX_INLINE void drawCall(Cmd_DrawCall& cmd) { onDrawCall(cmd); }

protected:
	Rect2f _scissorRect{0,0,0,0};

	AX_RENDER_CommandBuffer_FunctionInterfaces(=0)
};

}