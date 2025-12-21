module;
module AxRender;
import :RenderRequest_Backend;
import :Renderer_Backend;
import :Material_Backend;
import :RenderPass;
import :GpuBuffer;
import :RenderContext_Backend;
import :ResourceManager_Backend;

namespace ax /*::AxRender*/ {

UPtr<RenderRequest_Backend> RenderRequest_Backend::s_new(const MemAllocRequest& req, Renderer* renderer, Int index) {
	CreateDesc desc;
	desc.renderer = renderer;
	desc.index = index;
	auto o = Renderer_Backend::s_instance()->newRenderRequest(req, desc);
	AX_ASSERT(o->inlineUpload._stagingToGpuBuffer);
	return o;
}

RenderRequest_Backend::RenderRequest_Backend(const CreateDesc& desc) {
	_renderer = desc.renderer;
	_index = desc.index;
	inlineUpload.create(this);
}

void RenderRequest_Backend::waitCompleted() {
	onWaitCompleted();
	resourcesToKeep.clear();
	resourcesToUpdateDescriptor.clear();
}

void RenderRequest_Backend::waitCompletedAndReset(RenderSeqId newRenderSeqId) {
	waitCompleted();
	_renderSeqId = newRenderSeqId;
	resourcesToKeep.clear();
	inlineUpload.reset();
}

void RenderRequest_Backend::_updateCommonMaterial() {
	using namespace Math;

	auto* renderer_ = renderer_backend();
	if (!renderer_) throw Error_Undefined();

	
	auto* commonMaterial = renderer_->commonMaterial();
	if (!commonMaterial) throw Error_Undefined();

	resourcesToKeep.add(commonMaterial);

	_uptime = renderer_->getCurrentUptime().seconds_f64();
	
	f32 t = static_cast<f32>(_uptime);
	Vec4f timeSin		(sin(t),   sin(t*4), sin(t*9), sin(t*16));
	Vec4f timeSlowSin   (sin(t/2), sin(t/4), sin(t/9), sin(t/16));

	auto setParam = [&](BindSpace space, NameId name, auto& value) {
		if (!commonMaterial->setParamSpaceParam(space, name, value)) {
			commonMaterial->logWarningOnce(Fmt("Material: failure to setParam({}, {})", space, name));
		}
	};

	setParam(BindSpace::PerFrame, AX_NAMEID("ax_g_time"       ), t);
	setParam(BindSpace::PerFrame, AX_NAMEID("ax_g_timeSin"    ), timeSin);
	setParam(BindSpace::PerFrame, AX_NAMEID("ax_g_timeSlowSin"), timeSlowSin);
}

void RenderRequest_Backend::frameBegin(RenderContext_Backend* renderContext, RenderPass_Backend* backBufferRenderPass) {
	renderContext->imgui.onBeginRender(backBufferRenderPass->frameSize()); // TODO: move to frame update, to enable draw ui in update loop as well
	ResourceManager_Backend::s_instance()->onFrameBegin(this);

	_renderContext        = renderContext;
	_viewportIsBottomUp   = renderContext->viewportIsBottomUp();
	_frameSize            = backBufferRenderPass->frameSize();
	_backBufferRenderPass = backBufferRenderPass;

	_updateCommonMaterial();
	onFrameBegin();
}

void RenderRequest_Backend::frameEnd() {
	renderContext_backend()->imgui.onEndRender();
	ResourceManager_Backend::s_instance()->onFrameEnd(this);

	if (auto* r = rttiCastCheck<Renderer_Backend>(_renderer)) {
		r->onUpdateBindlessTables(this);
	}

	onFrameEnd();
}

void RenderRequest_Backend::renderPassBegin(RenderPass_Backend* pass) {
	_currentRenderPass = pass;
	resourcesToKeep.add(pass);

	auto s = pass->frameSize();
	Rect2f rect(Vec2f(0, 0), Vec2f::s_cast(s));

	setViewport(Rect2f::s_cast(rect), 0, 1);
	setScissorRect(rect);
	onRenderPassBegin(pass);
}

void RenderRequest_Backend::renderPassEnd(RenderPass_Backend* pass) {
	AX_ASSERT(_currentRenderPass == pass);
	onRenderPassEnd(pass);
	_currentRenderPass = nullptr;
	_viewportRect.set(0,0,0,0);
	_scissorRect.set(0,0,0,0);
}

void RenderRequest_Backend::setViewport_backend(const Rect2f& rect, float minDepth, float maxDepth) {
	if (Math::exactlyEqual(_viewportRect, rect))
		return;
	_viewportRect = rect;
	onSetViewport(rect, minDepth, maxDepth);
	setScissorRect(rect);
}

void RenderRequest_Backend::setScissorRect_backend(const Rect2f& rect) {
	if (Math::exactlyEqual(_scissorRect, rect))
		return;
	_scissorRect = rect;
	onSetScissorRect(rect);
}

void RenderRequest_Backend::copyDataToGpuBuffer(GpuBuffer* dst, ByteSpan data, Int dstOffset) {
	// TODO: don't know why doesn't work on DX12
	// if (inlineCopyDataToGpuBuffer(dst, data, dstOffset)) {
	// 	return;
	// }
	
	static auto copyGpuBufferAlignment = Renderer::s_instance()->copyGpuBufferAlignment();
	if (!Math::isAlignedTo(data.size(), copyGpuBufferAlignment)) throw Error_Undefined(); 
	if (!Math::isAlignedTo(dstOffset,   copyGpuBufferAlignment)) throw Error_Undefined(); 

// use upload buffer
	auto uploadBuf = GpuBuffer_Backend::s_new(AX_ALLOC_REQ,
											  Fmt("{}-upload", _name),
											  GpuBufferType::StagingToGpu,
											  data.size());

	resourcesToKeep.add(uploadBuf.ptr());
	uploadBuf->copyData(data);

	auto* dstBuffer = rttiCastCheck<GpuBuffer_Backend>(dst);
	dstBuffer->copyFromGpuBuffer(this, uploadBuf, uploadBuf->bufferRange(), dstOffset);
	
}

void RenderRequest_Backend::drawUI_backend() {
	renderContext_backend()->imgui.onDrawUI(this);
}

void RenderRequest_Backend::drawCall_backend(Cmd_DrawCall& cmd) {
	if (cmd.instanceCount <= 0) { AX_ASSERT(false); return; }

	auto* material = rttiCastCheck<Material_Backend>(cmd.material);
	if (!material) { AX_ASSERT(false); return; }

	auto* matPass = material->getPass(cmd.materialPassIndex);
	if (!matPass) { AX_ASSERT(false); return; } // TODO use dummy shader instead

	resourcesToKeep.add(material);

	if (auto* vertexBuffer = rttiCastCheck<GpuBuffer_Backend>(cmd.vertexBuffer)) {
		resourcesToKeep.add(vertexBuffer);
	}

	if (auto* indexBuffer = rttiCastCheck<GpuBuffer_Backend>(cmd.indexBuffer)) {
		resourcesToKeep.add(indexBuffer);
	}

	matPass->onDrawcall(this, cmd);

	onDrawCall(cmd);
}

bool RenderRequest_Backend::inlineCopyDataToGpuBuffer(GpuBuffer* dst, ByteSpan data, Int dstOffset) {
	static auto copyGpuBufferAlignment = Renderer::s_instance()->copyGpuBufferAlignment();
	if (!Math::isAlignedTo(data.size(), copyGpuBufferAlignment)) throw Error_Undefined();
	if (!Math::isAlignedTo(dstOffset,   copyGpuBufferAlignment)) throw Error_Undefined();
	if (!Math::isAlignedTo(inlineUpload._used, copyGpuBufferAlignment)) throw Error_Undefined();

	auto dataSize = data.size();
	if (dataSize > inlineUpload._limitPerEach) return false;
	if (dataSize > inlineUpload.remainSize()) return false;

	auto& uploadBuf = inlineUpload._stagingToGpuBuffer;
	
	resourcesToKeep.add(uploadBuf.ptr());
	uploadBuf->copyData(data, inlineUpload._used);

	auto* dstBuffer = rttiCastCheck<GpuBuffer_Backend>(dst);

	IntRange uploadRange = IntRange::s_beginSize(inlineUpload._used, dataSize);
	dstBuffer->copyFromGpuBuffer(this, uploadBuf, uploadRange, dstOffset);

	inlineUpload._used += dataSize;
	return true;
}

void RenderRequest_Backend::InlineUpload::create(RenderRequest_Backend* req) {
	_req = req;

	auto& info = req->renderer()->info().inlineUpload;

	_limitPerEach = info.limitPerEach;
	if (info.bufferSize <= 0) return;

	_stagingToGpuBuffer = GpuBuffer_Backend::s_new(AX_ALLOC_REQ,
	                                               "InlineBuffer",
	                                               GpuBufferType::StagingToGpu,
	                                               info.bufferSize);
}

} // namespace ax /*::AxRender*/