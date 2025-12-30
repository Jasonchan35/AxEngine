module AxRender;
import :RenderContext;
import :RenderMesh;
import :RenderRequest_Backend;

namespace ax /*::AxRender*/ {

void RenderRequest::drawCall(Cmd_DrawCall& cmd) {
	static_cast<RenderRequest_Backend*>(this)->drawCall_backend(cmd);
}

void RenderRequest::drawUI() {
	static_cast<RenderRequest_Backend*>(this)->drawUI_backend();
}

void RenderRequest::setViewport(const Rect2f& rect, float minDepth, float maxDepth) {
	static_cast<RenderRequest_Backend*>(this)->setViewport_backend(rect, minDepth, maxDepth);
}

void RenderRequest::setScissorRect(const Rect2f& rect) {
	static_cast<RenderRequest_Backend*>(this)->setScissorRect_backend(rect);
}


} // namespace