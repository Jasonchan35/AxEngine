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
	_name = NameId::s_make(Fmt("RenderRequest#{}", desc.index));
	_renderSystem         = desc.renderSystem;
	_renderSystem_backend = desc.renderSystem;
	_index                = desc.index;
	_inlineUpload.create(this);
	_stockObjects         = RenderStockObjects::s_instance();
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

void RenderRequest_Backend::_updateGlobalCommonMaterial() {
	using namespace Math;
	_globalCommonMaterial     = Material_Backend::s_globalCommonMaterial();
	_globalCommonMaterialPass = _globalCommonMaterial->getPass(0);
	resourcesToKeep.add(_globalCommonMaterial);
	
	f32 t = static_cast<f32>(_uptime);
	Vec4f timeSin		(sin(t),   sin(t*4), sin(t*9), sin(t*16));
	Vec4f timeSlowSin   (sin(t/2), sin(t/4), sin(t/9), sin(t/16));

	auto setParam = [&](BindSpace space, NameId name, auto& value) {
		if (!_globalCommonMaterial->setParam(space, name, value)) {
			_globalCommonMaterial->logWarningOnce(Fmt("Material: failure to setParam({}, {})", space, name));
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

	_renderContext        = renderContext;
	_renderGraph          = renderContext->renderGraph();
	_projectionDesc       = _renderGraph->projectionDesc();
	_frameSize            = backBufferRenderPass->frameSize();
	_backBufferRenderPass = backBufferRenderPass;
	_uptime               = _renderSystem_backend->getCurrentUptime().seconds_f64();
	_objectManager        = RenderObjectManager_Backend::s_instance();
	_updateGlobalCommonMaterial();
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
	_cameraData.worldPos          = camera.eye();
	_cameraData.viewportMin       = camera.viewport.min();
	_cameraData.viewportMax       = camera.viewport.max();
	
	_cameraData.projMatrix        = camera.projMatrix(_projectionDesc);
	_cameraData.projMatrixInv     = _cameraData.projMatrix.inverse();
	_cameraData.viewMatrix        = camera.viewMatrix(_projectionDesc);
	_cameraData.viewMatrixInv     = _cameraData.viewMatrix.inverse();
	_cameraData.viewProjMatrix    = camera.viewProjMatrix(_projectionDesc);
	_cameraData.viewProjMatrixInv = _cameraData.viewProjMatrix.inverse();
	
	auto* matPass = MaterialPass_Backend::s_globalCommonMaterialPass();
	if (!matPass) { AX_ASSERT(false); return; }
	auto* paramSpace = matPass->getOwnParamSpace(BindSpace::World);
	if (!paramSpace) { AX_ASSERT(false); return; }
	
	auto* cb = paramSpace->constBuffer_camera();
	if (!cb) { AX_ASSERT(false); return; }
	
	cb->setVariable(AX_NAMEID("worldPos"         ), _cameraData.worldPos         );
	cb->setVariable(AX_NAMEID("viewportMin"      ), _cameraData.viewportMin      ); 
	cb->setVariable(AX_NAMEID("viewportMax"      ), _cameraData.viewportMax      ); 
	cb->setVariable(AX_NAMEID("projMatrix"       ), _cameraData.projMatrix       ); 
	cb->setVariable(AX_NAMEID("projMatrixInv"    ), _cameraData.projMatrixInv    ); 
	cb->setVariable(AX_NAMEID("viewMatrix"       ), _cameraData.viewMatrix       ); 
	cb->setVariable(AX_NAMEID("viewMatrixInv"    ), _cameraData.viewMatrixInv    ); 
	cb->setVariable(AX_NAMEID("viewProjMatrix"   ), _cameraData.viewProjMatrix   ); 
	cb->setVariable(AX_NAMEID("viewProjMatrixInv"), _cameraData.viewProjMatrixInv);
	
	// force update to GPU
	cb->getUploadedGpuBuffer(this);
}

void RenderRequest_Backend::copyDataToGpuBuffer_StagingBuffer(GpuBuffer* dst, ByteSpan data, Int dstOffset) {
	auto uploadBuf = GpuBuffer_Backend::s_new(AX_NEW,
											  Fmt("{}-upload", _name),
											  GpuBufferType::StagingToGpu,
											  data.size());

	resourcesToKeep.add(uploadBuf.ptr());
	uploadBuf->copyData(data);

	auto* dstBuffer = rttiCastCheck<GpuBuffer_Backend>(dst);
	dstBuffer->copyFromGpuBuffer(this, uploadBuf, IntRange(uploadBuf->size()), dstOffset);
}

bool RenderRequest_Backend::copyDataToGpuBuffer_InlineBuffer(GpuBuffer* dst, ByteSpan data, Int dstOffset) {
	auto dataSize = data.size();
	if (dataSize > _inlineUpload.maxSizePerUpload) return false;
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

void RenderRequest_Backend::vertexShaderDraw_backend(AxVertexShaderDraw& draw) {
	if (draw.instanceCount <= 0) { AX_ASSERT(false); return; }
	
	auto* material = rttiCastCheck<Material_Backend>(draw.material);
	if (!material) { AX_ASSERT(false); return; }
	resourcesToKeep.add(material);

	if (auto* vertexBuffer = rttiCastCheck<GpuBuffer_Backend>(draw.vertexBuffer)) {
		resourcesToKeep.add(vertexBuffer);
	}

	if (auto* indexBuffer = rttiCastCheck<GpuBuffer_Backend>(draw.indexBuffer)) {
		resourcesToKeep.add(indexBuffer);
	}

	auto* matPass = material->getPass(draw.materialPassIndex);
	if (!matPass) { AX_ASSERT(false); return; } // TODO use dummy shader instead
	
	AxVertexShaderDrawRootConst rootConst;
	rootConst.worldMatrix = draw.objectToWorld;
	matPass->onBindMaterial(this, draw, &rootConst);
	
	onVertexShaderDraw(draw);
}

void RenderRequest_Backend::meshShaderDraw_backend(AxMeshShaderDraw& draw) {
	auto* meshObject = rttiCastCheck<MeshObject_Backend>(draw.meshObject);
	resourcesToKeep.add(meshObject);
	
	auto* material = rttiCastCheck<Material_Backend>(draw.material);
	if (!material) { AX_ASSERT(false); return; }
	resourcesToKeep.add(material);

	auto* matPass = material->getPass(draw.materialPassIndex);
	if (!matPass) { AX_ASSERT(false); return; } // TODO use dummy shader instead
	
	auto lodGroupSpan = meshObject->meshLodGroup.readData();
	
	AxMeshShaderDrawRootConst rootConst;
	rootConst.worldMatrix    = draw.objectToWorld;
	rootConst.meshLodGroupId = ax_safe_cast_from(draw.lodGroupId + meshObject->meshLodGroup.buffer->gpuBufferIndex());
	matPass->onBindMaterial(this, draw, &rootConst);
	
	if (!matPass->isMeshShader()) throw Error_Undefined("expect mesh shader to draw mesh object");
	onMeshShaderDraw(draw);
}

void RenderRequest_Backend::drawMeshRenderer_backend(MeshObjectRenderer* mr) {
	if (!mr) return;
//	_meshShaderDrawRootConst.AX_MESH_RENDERER_ID = mr->objectSlot.slotId();
}

void RenderRequest_Backend::InlineUpload::create(RenderRequest_Backend* req) {
	auto& info = req->renderSystem()->info().inlineUpload;

	maxSizePerUpload = info.maxSizePerUpload;
	if (info.maxBufferSize <= 0) return;

	GpuBuffer_CreateDesc bufDesc;
	bufDesc.name = FmtName("{}-InlineUpload", req->name());
	bufDesc.bufferType = GpuBufferType::StagingToGpu;
	bufDesc.bufferSize = info.maxBufferSize;

	stagingToGpuBuffer = GpuBuffer_Backend::s_new(AX_NEW, bufDesc);
}

} // namespace ax /*::AxRender*/