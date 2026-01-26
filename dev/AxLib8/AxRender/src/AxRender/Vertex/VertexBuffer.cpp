module AxRender;

namespace ax /*::AxRender*/ {

void VertexBuffer::create(VertexLayout vertexLayout, const GpuVirtualMemoryDesc& virMemDesc) {
	if (!vertexLayout)
		throw Error_Undefined();

	_vertexLayout = vertexLayout;
	_vertexCount = 0;

	DynamicGpuBuffer_CreateDesc desc;
	desc.bufferType = GpuBufferType::Vertex;
	desc.name		= "VertexBuffer";
	desc.virMemDesc = virMemDesc;
	_buffer.create(desc);
}

void VertexIndexBuffer::create(VertexIndexType indexType, const GpuVirtualMemoryDesc& virMemDesc) {
	_indexType = indexType;
	_indexCount = 0;

	if (_indexType == VertexIndexType::None) {
		_buffer.destroy();
	} else {
		DynamicGpuBuffer_CreateDesc desc;
		desc.bufferType = GpuBufferType::Index;
		desc.name		= "IndexBuffer";
		desc.virMemDesc = virMemDesc;
		_buffer.create(desc);
	}
}

} // namespace