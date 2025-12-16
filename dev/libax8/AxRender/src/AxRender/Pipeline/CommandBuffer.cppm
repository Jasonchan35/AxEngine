module;
export module AxRender:CommandBuffer;
export import :RenderDataType;
export import :VertexLayout;
export import :GpuBuffer;
export import :Material;
export import :RenderPass;
export import :RenderMesh;

export namespace ax /*::AxRender*/ {

class Cmd_DrawCall : public NonCopyable {
public:
	RenderPrimitiveType	primitiveType	= RenderPrimitiveType::Triangles;
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

	void setSubMesh(RenderRequest* req, RenderSubMesh& sm) {
		primitiveType = sm.primitiveType();
		setVertexBuffer(req, sm.vertexBuffer);
		setIndexBuffer( req, sm.indexBuffer);
	}

	void setVertexBuffer(RenderRequest* req, VertexBuffer& vb) {
		vertexLayout = vb.vertexLayout();
		vertexCount  = vb.vertexCount();
		vertexBuffer = vb.getUploadedGpuBuffer(req);
	}

	void setIndexBuffer(RenderRequest* req, IndexBuffer& ib) {
		indexType    = ib.indexType();
		indexCount   = ib.indexCount();
		indexBuffer  = ib.getUploadedGpuBuffer(req);
	}

	void setMaterialPass(Material* material_, Int passIndex) {
		material = material_;
		materialPassIndex = passIndex;
	}
};

enum class CommandBufferType {
	None,
	Direct,
	Bundle,
	Compute,
	Copy,
	VideoDecode,
	VideoProcess,
	VideoEncode,
};

struct CommandBuffer_CreateDesc {
	CommandBufferType type = CommandBufferType::None;
};

class CommandBuffer : public RenderObject {
	AX_RTTI_INFO(CommandBuffer, RenderObject)
public:
};

}