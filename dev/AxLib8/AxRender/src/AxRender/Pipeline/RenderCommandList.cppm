module;
export module AxRender:RenderCommandList;
export import :RenderDataType;
export import :VertexLayout;
export import :GpuBuffer;
export import :Material;
export import :RenderPass;
export import :RenderMesh;

export namespace ax /*::AxRender*/ {

class Cmd_DrawCall : public NonCopyable {
public:
	RenderPrimitiveType primitiveType     = RenderPrimitiveType::Triangles;
	VertexIndexType     indexType         = VertexIndexType::None;
	VertexLayout        vertexLayout      = nullptr;
	const GpuBuffer*    vertexBuffer      = nullptr;
	Int                 vertexStart       = 0;
	Int                 vertexCount       = 0;
	const GpuBuffer*    indexBuffer       = nullptr;
	Int                 indexStart        = 0;
	Int                 indexCount        = 0;
	Int                 instanceStart     = 0;
	Int                 instanceCount     = 1;
	Material*           material          = nullptr;
	Int                 materialPassIndex = 0;
	Mat4f               objectToWorld     = Mat4f::s_identity();

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

	void setIndexBuffer(RenderRequest* req, VertexIndexBuffer& ib) {
		indexType    = ib.indexType();
		indexCount   = ib.indexCount();
		indexBuffer  = ib.getUploadedGpuBuffer(req);
	}

	void setMaterialPass(Material* material_, Int passIndex) {
		material = material_;
		materialPassIndex = passIndex;
	}
};

enum class RenderCommandListType {
	None,
	Direct,
	Bundle,
	Compute,
	Copy,
	VideoDecode,
	VideoProcess,
	VideoEncode,
};

struct RenderCommandList_CreateDesc {
	RenderCommandListType type = RenderCommandListType::None;
};

class RenderCommandList : public RenderObject {
	AX_RTTI_INFO(RenderCommandList, RenderObject)
public:
};

}