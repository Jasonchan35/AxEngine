module;
module AxRender;
import :RenderRequest_Backend;
import :RenderSystem_Backend;
import :Material_Backend;
import :RenderPass;
import :GpuBuffer;
import :RenderContext_Backend;
import :RenderObjectManager_Backend;

namespace ax /*::AxRender*/ {

UPtr<RenderRequest_Backend> RenderRequest_Backend::s_new(const MemAllocRequest& req, RenderSystem_Backend* renderSystem, Int index) {
	CreateDesc desc;
	desc.renderSystem = renderSystem;
	desc.index = index;
	return RenderSystem_Backend::s_instance()->newRenderRequest(req, desc);
}

RenderRequest_Backend::RenderRequest_Backend(const CreateDesc& desc) {
	_renderSystem         = desc.renderSystem;
	_renderSystem_backend = desc.renderSystem;
	_index                = desc.index;
	_inlineUpload.create(this);
	_renderRequestCount = _renderSystem->renderRequestCount();
	_stockObjects = RenderStockObjects::s_instance();
}

void RenderRequest_Backend::waitCompleted() {
	onWaitCompleted();
	resourcesToKeep.clear();
}

void RenderRequest_Backend::waitCompletedAndReset(RenderSeqId newRenderSeqId) {
	waitCompleted();
	_renderSeqId = newRenderSeqId;
	resourcesToKeep.clear();
	_inlineUpload.reset();
}

void RenderRequest_Backend::_updateCommonMaterial() {
	using namespace Math;

	_commonMaterial     = rttiCastCheck<Material_Backend>(_stockObjects->commonMaterial.ptr());
	_commonMaterialPass = _commonMaterial->getPass(0);
	resourcesToKeep.add(_commonMaterial);
	
	f32 t = static_cast<f32>(_uptime);
	Vec4f timeSin		(sin(t),   sin(t*4), sin(t*9), sin(t*16));
	Vec4f timeSlowSin   (sin(t/2), sin(t/4), sin(t/9), sin(t/16));

	auto setParam = [&](BindSpace space, NameId name, auto& value) {
		if (!_commonMaterial->setParam(space, name, value)) {
			_commonMaterial->logWarningOnce(Fmt("Material: failure to setParam({}, {})", space, name));
		}
	};

	setParam(BindSpace::World, AX_NAMEID("ax_g_time"       ), t);
	setParam(BindSpace::World, AX_NAMEID("ax_g_timeSin"    ), timeSin);
	setParam(BindSpace::World, AX_NAMEID("ax_g_timeSlowSin"), timeSlowSin);
}

void RenderRequest_Backend::frameBegin(RenderContext_Backend* renderContext, RenderPass_Backend* backBufferRenderPass) {
	//	AX_LOG("---- FrameBegin {} -----", _renderSeqId);

	// TODO: move to frame update, to enable draw ui in update loop as well
	renderContext->imgui.onBeginRender(backBufferRenderPass->frameSize());
	RenderObjectManager_Backend::s_instance()->onFrameBegin(this);

	_renderContext         = renderContext;
	_frameSize             = backBufferRenderPass->frameSize();
	_backBufferRenderPass  = backBufferRenderPass;
	_uptime                = _renderSystem_backend->getCurrentUptime().seconds_f64();
	_objectManager = RenderObjectManager_Backend::s_instance();
	_updateCommonMaterial();
	onFrameBegin();
}

void RenderRequest_Backend::frameEnd() {
	renderContext_backend()->imgui.onEndRender();
	RenderObjectManager_Backend::s_instance()->onFrameEnd(this);
	onFrameEnd();
//	AX_LOG("---- FrameEnd {} -----", _renderSeqId);
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

void RenderRequest_Backend::setCamera_backend(const Math::Camera3f& camera) {
	_viewProjMatrix = camera.viewProjMatrix();
	commonMaterialPass()->setParam(ShaderParamBindSpace::Object, AX_NAMEID("ax_object_vp"), _viewProjMatrix);
}

void RenderRequest_Backend::copyDataToGpuBuffer_StagingBuffer(GpuBuffer* dst, ByteSpan data, Int dstOffset) {
	auto uploadBuf = GpuBuffer_Backend::s_new(AX_NEW,
											  Fmt("{}-upload", _name),
											  GpuBufferType::StagingToGpu,
											  data.size());

	resourcesToKeep.add(uploadBuf.ptr());
	uploadBuf->copyData(data);

	auto* dstBuffer = rttiCastCheck<GpuBuffer_Backend>(dst);
	dstBuffer->copyFromGpuBuffer(this, uploadBuf, uploadBuf->bufferRange(), dstOffset);
}

bool RenderRequest_Backend::copyDataToGpuBuffer_InlineBuffer(GpuBuffer* dst, ByteSpan data, Int dstOffset) {
	auto dataSize = data.size();
	if (dataSize > _inlineUpload.limitPerEach) return false;
	if (dataSize > _inlineUpload.remainSize()) return false;

	auto& uploadBuf = _inlineUpload.stagingToGpuBuffer;
	uploadBuf->copyData(data, _inlineUpload.usedBytes);

	auto* dstBuffer = rttiCastCheck<GpuBuffer_Backend>(dst);

	auto uploadRange = IntRange_StartAndSize(_inlineUpload.usedBytes, dataSize);
	dstBuffer->copyFromGpuBuffer(this, uploadBuf, uploadRange, dstOffset);

	_inlineUpload.usedBytes += dataSize;
	return true;
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
	
	rootConstStruct.mvp = _viewProjMatrix * cmd.objectToWorld;

	matPass->onBindMaterial(this, cmd);
	onDrawCall(cmd);
}

void RenderRequest_Backend::InlineUpload::create(RenderRequest_Backend* req) {
	auto& info = req->renderSystem()->info().inlineUpload;

	limitPerEach = info.limitPerEach;
	if (info.bufferSize <= 0) return;

	stagingToGpuBuffer = GpuBuffer_Backend::s_new(AX_NEW,
	                                               Fmt("{}-InlineUpload", req->name()),
	                                               GpuBufferType::StagingToGpu,
	                                               info.bufferSize);
}

} // namespace ax /*::AxRender*/