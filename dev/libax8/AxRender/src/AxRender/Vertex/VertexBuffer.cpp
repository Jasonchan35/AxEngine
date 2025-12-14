module AxRender;

namespace ax /*::AxRender*/ {

void VertexBuffer::create(VertexLayout vertexLayout) {
	if (!vertexLayout)
		throw Error_Undefined();

	_vertexLayout = vertexLayout;
	_vertexCount = 0;

	DynamicGpuBuffer_CreateDesc desc;
	desc.bufferType = GpuBufferType::Vertex;
	desc.name		= "VertexBuffer";
	_buffer.create(desc);
}

void IndexBuffer::create(IndexType indexType) {
	if (indexType == IndexType::None)
		throw Error_Undefined();

	_indexType = indexType;
	_indexCount = 0;

	DynamicGpuBuffer_CreateDesc desc;
	desc.bufferType = GpuBufferType::Index;
	desc.name		= "IndexBuffer";
	_buffer.create(desc);
}

} // namespace