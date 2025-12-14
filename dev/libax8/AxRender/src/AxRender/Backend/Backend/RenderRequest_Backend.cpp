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
	return rttiCast<RenderRequest_Backend>(Renderer_Backend::s_instance()->newRenderRequest(req, desc));
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

void RenderRequest_Backend::waitCompletedAndReset(RenderSeqId renderSeqId) {
	waitCompleted();
	_renderSeqId = renderSeqId;
	resourcesToKeep.clear();
	inlineUpload.reset();
}

void RenderRequest_Backend::frameBegin(RenderContext_Backend* renderContext, RenderPass_Backend* backBufferRenderPass) {
	_renderContext = renderContext;
	_viewportIsBottomUp = renderContext->viewportIsBottomUp();
	_frameSize = backBufferRenderPass->frameSize();
	_backBufferRenderPass = backBufferRenderPass;

	auto* renderer_ = renderer_backend();
	if (!renderer_) throw Error_Undefined();

	_uptime = renderer_->getCurrentUptime().seconds_f64();

//---- update common material
	{
		using namespace Math;

		auto* commonMaterial = renderer_->commonMaterial();
		if (!commonMaterial) throw Error_Undefined();

		resourcesToKeep.add(commonMaterial);

		f32 t = static_cast<f32>(_uptime);
		Vec4f timeSin		(sin(t),   sin(t*4), sin(t*9), sin(t*16));
		Vec4f timeSlowSin   (sin(t/2), sin(t/4), sin(t/9), sin(t/16));

		auto setParam = [&](ParamSpaceType space, NameId name, auto& value) {
			if (!commonMaterial->setParamSpaceParam(space, name, value)) {
				commonMaterial->logWarningOnce(Fmt("Material: failure to setParam({}, {})", space, name));
			}
		};

		setParam(ParamSpaceType::PerFrame, AX_NAMEID("ax_g_time"       ), t);
		setParam(ParamSpaceType::PerFrame, AX_NAMEID("ax_g_timeSin"    ), timeSin);
		setParam(ParamSpaceType::PerFrame, AX_NAMEID("ax_g_timeSlowSin"), timeSlowSin);
	}

	onFrameBegin();
}

void RenderRequest_Backend::renderPassBegin(RenderPass_Backend* pass) {
	_currentRenderPass = pass;
	resourcesToKeep.add(pass);

	auto s = pass->frameSize();
	Rect2f rect(Vec2f(0, 0), Vec2f::s_cast(s));

	setViewport(Rect2f::s_cast(rect), 0, 1);
	_graphCmdBuf->setScissorRect(rect);
	_graphCmdBuf->renderPassBegin(pass);

	onRenderPassBegin();
}

void RenderRequest_Backend::renderPassEnd() {
	onRenderPassEnd();

	_graphCmdBuf->renderPassEnd();
	_currentRenderPass = nullptr;
}

void RenderRequest_Backend::onDrawcall(Cmd_DrawCall& cmd) {
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
	_graphCmdBuf->drawCall(cmd);
}

void RenderRequest_Backend::frameEnd() {
	renderContext_backend()->imgui.onEndRender();

	ResourceManager_Backend::s_instance()->onFrameEnd(this);

	if (auto* r = rttiCastCheck<Renderer_Backend>(_renderer)) {
		r->onUpdateBindlessTables(this);
	}

	onFrameEnd();
}

void RenderRequest_Backend::onDrawUI() {
	renderContext_backend()->imgui.onDrawUI(this);
}

bool RenderRequest_Backend::InlineUpload::copyDataToGpuBuffer(GpuBuffer_Backend* dst, ByteSpan data, Int dstOffset) {
	if (!dst) return false;

	auto dataSize = data.size();

	if (dataSize > _limitPerEach) return false;
	if (dataSize > remainSize()) return false;

	IntRange uploadRange = IntRange::s_beginSize(_used, dataSize);

	{
		auto map = _gpuBuffer->mapMemory(uploadRange);
		map->copyValues(data, 0);
	}

	_used += dataSize;

	dst->copyFromGpuBuffer(_req, _gpuBuffer, uploadRange, dstOffset);
	return true;
}

void RenderRequest_Backend::InlineUpload::create(RenderRequest_Backend* req) {
	_req = req;

	auto& info = req->renderer()->info().inlineUpload;

	_limitPerEach = info.limitPerEach;
	if (info.bufferSize <= 0) return;

	_gpuBuffer = GpuBuffer_Backend::s_new(AX_ALLOC_REQ, "InlineBuffer", GpuBufferType::StagingToGpu, info.bufferSize);
}

} // namespace ax /*::AxRender*/