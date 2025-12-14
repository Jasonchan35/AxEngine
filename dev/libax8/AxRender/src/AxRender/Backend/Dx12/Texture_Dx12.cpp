module AxRender;
import :Texture_Dx12;

#if AX_RENDERER_DX12

namespace ax {

void Texture2D_Dx12::onImageIO_ReadHandler(ImageIO_ReadHandler& handler) {
	// TODO
	auto dataSize = handler.desc.dataSize;
	ByteArray buf;
	buf.resize(dataSize);
	handler.readPixelsTo(buf);
}

} // namespace

#endif //AX_RENDERER_DX12
