module AxRender;

namespace ax /*::AxRender*/ {

void VertexBuffer::create(VertexLayout vertexLayout) {
	if (!vertexLayout)
		throw Error_Undefined();

	_vertexLayout = vertexLayout;
	_vertexCount = 0;

	DynamicGpuBuffer_CreateDesc desc;
	desc.bufferType = GpuBufferType::Vertex;
	desc.name       = AX_NAMEID("VertexBuffer");
	desc.pool       = RenderObjectManager_Backend::s_instance()->_bufferPools.vertex.ptr();
	_buffer.create(desc);
}

void VertexIndexBuffer::create(VertexIndexType indexType) {
	_indexType = indexType;
	_indexCount = 0;

	if (_indexType == VertexIndexType::None) {
		_buffer.destroy();
	} else {
		DynamicGpuBuffer_CreateDesc desc;
		desc.bufferType = GpuBufferType::Index;
		desc.name		= AX_NAMEID("IndexBuffer");
		desc.pool       = RenderObjectManager_Backend::s_instance()->_bufferPools.index.ptr();
		_buffer.create(desc);
	}
}

} // namespace