module AxRender;
import :RenderRequest;
import :RenderContext;
import :RenderRequest_Backend;
import :Renderer_Backend;
import :RenderContext_Backend;
import :RenderPass_Backend;

namespace ax::AxRender {

RenderGraph_ColorBuffer::RenderGraph_ColorBuffer( Pass* pass, StrView name, const RenderColorBufferDesc& desc)
	: _pass(pass)
	, _name(name)
	, _desc(desc)
{
	pass->_addColorBuffer(this);
}

void RenderGraph_ColorBuffer::setDesc(const RenderColorBufferDesc& desc) {
	if (_desc == desc) return;
	setDirty();
	_desc = desc;
}

void RenderGraph_ColorBuffer::setClearColor(const Color4f& color) {
	if (_desc.clearColor == color) return;
	_desc.clearColor = color;
	setDirty();
}

void RenderGraph_ColorBuffer::setColorType(ColorType colorType) {
	if (_desc.colorType == colorType) return;
	_desc.colorType = colorType;
	setDirty();
}

void RenderGraph_ColorBuffer::setLoadOp(RenderBufferLoadOp loadOp) {
	if (_desc.loadOp == loadOp) return;
	_desc.loadOp = loadOp;
	setDirty();
}

void RenderGraph_Pass::setDepthBufferDesc(const RenderDepthBufferDesc& desc) {
	if (_depthBufferDesc == desc)
		return;

	setDirty();
	_depthBufferDesc = desc;
}

void RenderGraph_Pass::_createRenderPass() {
	if (!_isBackBuffer) return;

	RenderPass_CreateDesc	desc;
	desc.frameSize = _frameSize;
	desc.depthBuffer = _depthBufferDesc;
		
	for (auto& col : _colorBuffers) {
		if (!col) { AX_ASSERT(false); return; }
		desc.colorBuffers.emplaceBack(col->desc());
	}

	if (_renderPass && _renderPass->isCompatible(desc))
		return;

	desc.name = _name;

	_renderPass = RenderPass_Backend::s_new(AX_ALLOC_REQ, desc);
}

void RenderGraph_Pass::_render(RenderRequest_Backend* req) {
	RenderPass_Backend* outPass = nullptr;
	if (_isBackBuffer) {
		outPass = req->backBufferRenderPass();
	} else {
		outPass = rttiCastCheck<RenderPass_Backend>(_renderPass.ptr());
	}

	AX_ASSERT(outPass);
//	AX_LOG("renderPass {} -> {}", name(), outRenderPass->name());
	req->renderPassBegin(outPass);
	if (_renderDelegate) {
		_renderDelegate.invoke(req, _inputs);
	}
	req->renderPassEnd();
}

void RenderGraph_Pass::_init(RenderGraph* graph, StrView name) {
	_graph = graph;
	_name = name;
	_graph->_addPass(this);
}

bool RenderGraph_Pass::_compareInputs(Span<ColorBuffer*> inputs) {
	if (inputs.size() != _inputs.size())
		return false;

	Int n = _inputs.size();
	for (Int i = 0; i < n; i++) {
		if (inputs[i] != _inputs[i].source())
			return false;
	}
	
	return true;
}

void RenderGraph_Pass::setInputs(Span<ColorBuffer*> inputs) {
	if (_compareInputs(inputs))
		return;

	_graph->setDirty();
	_inputs.clear();
	for (auto& src : inputs) {
		_inputs.emplaceBack(this, src);
	}
}

void RenderGraph_Pass::_addColorBuffer(ColorBuffer* colorBuf) {
	_colorBuffers.emplaceBack(colorBuf);
}

void RenderGraph_Pass::setFrameSize(Vec2i frameSize) {
	if (frameSize == _frameSize) return;
	setDirty();
	_frameSize = frameSize;
}

void RenderGraph_Pass::setAutoFrameSize(AutoFrameSize autoFrameSize) {
	if (_autoFrameSize == autoFrameSize) return;
	setDirty();
	_autoFrameSize = autoFrameSize;
}

void RenderGraph::setFrameSize(Vec2i frameSize) {
	if (_frameSize == frameSize) return;

	setDirty();
	_frameSize = frameSize;
}

void RenderGraph::_render(RenderRequest_Backend* req) {
	setFrameSize(req->frameSize());
	_backBufferPass.setRenderPass(req->backBufferRenderPass());

	onUpdate(req);

	if (!_rebuild()) return;

	for (auto& pass : _resultPasses) {
		pass->_render(req);
	}
}

bool RenderGraph::_rebuild() {
	if (!_dirty) return true;
	_dirty = false;

	auto& pending  = _buildData._pendingPasses;
	auto& depended = _buildData._dependedPasses;

	 pending.clear();
	depended.clear();
	_resultPasses.clear();

	if (_passes.size() <= 0) return false;

	      pending.ensureCapacity(_passes.size());
	     depended.ensureCapacity(_passes.size());
	_resultPasses.ensureCapacity(_passes.size());

// reset
	for (auto& pass : _passes) {
		if (!pass) { AX_ASSERT(false); return false; }

		if (pass->_autoFrameSize != AutoFrameSize::None) {
			auto newFrameSize = AutoFrameSize_Compute(pass->_autoFrameSize, _frameSize);
			pass->setFrameSize(newFrameSize);
		}

		pass->_buildData._added = false;
		pass->_buildData._leafLevel = Int_max;
		pass->_buildData._dependedCount = 0;
	}

// search depended on passes start from output pass
	pending.emplaceBack(&_backBufferPass);

	while (pending.size()) {
		auto* pass = pending.popBack();

		auto size = pass->frameSize();
		if (size.x <= 0 || size.y <= 0) { AX_ASSERT(false); return false; }

		for(auto& input : pass->_inputs) {
			auto* srcPass = input.sourcePass();
			if (!srcPass) { AX_ASSERT(false); return false; }

			srcPass->_buildData._dependedCount++;

			if (!srcPass->_buildData._added) {
				srcPass->_buildData._added = true;
				pending.emplaceBack(srcPass);
			}
		}

		depended.emplaceBack(pass);
	}

// pick leaf node first
	for (Int lv = 0; lv < _passes.size(); lv++) {
		Int found = 0;

		if (depended.size() <= 0) break;

		for (Int i = 0; i < depended.size(); i++) {
			auto* pass = depended[i];

			bool isLeaf = true;

			for (auto& input : pass->_inputs) {
				auto* srcPass = input.sourcePass();
				if (!srcPass) {
					AX_ASSERT(false);
					continue;
				}

				if (srcPass->_buildData._leafLevel >= lv) {
					isLeaf = false;
					break;
				}
			}

			if (isLeaf) {
				found++;
				pass->_buildData._leafLevel = lv;
				_resultPasses.emplaceBack(pass);
				depended.eraseAt_Unordered(i);
				i--;
			}
		}

		if (found <= 0) {
			// cycle dependency
			AX_ASSERT(false);
			return false;
		}
	}

//--- print result list
#if 0
	for (auto& p : resultList) {
		auto msg = Fmt("{}: {} - depended={} ", p->_leafLevel, p->name(), p->_dependedCount);
		for (auto& input : p->_inputs) {
			auto* src = input->source();
			FmtTo(msg, " {}.{}", src->pass()->name(), src->name());
		}
		AX_LOG("{}", msg);
	}
	AX_LOG("test");
#endif

	for (auto& pass : _resultPasses) {
		pass->_createRenderPass();
	}

	return true;
}

RenderGraph::RenderGraph() 
: _backBufferPass(this, &RenderGraph::onBackBufferPass) 
{
}

void RenderGraph_BackBufferPass::setRenderPass(RenderPass* pass) {
	_renderPass = nullptr;
	if (!pass) return;

	setFrameSize(pass->frameSize());
	auto* desc = pass->colorBufferDesc(0);
	if (!desc) return;

	color0.setDesc(*desc);
	_renderPass = pass;
}

} // namespace ax::AxRender