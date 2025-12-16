module AxRender;
import :RenderContext_Dx12;
import :RenderRequest_Dx12;

#if AX_RENDERER_DX12

namespace ax {

RenderContext_Dx12::RenderContext_Dx12(const CreateDesc& desc)
	: Base(desc)
	, _uiEventHandler(this) {
	_createWindow(desc);
}

void RenderContext_Dx12::_createSwapChain() {
	RECT clientRect;
	::GetClientRect(_hwnd, &clientRect);

	auto  frameSize   = Math::max(_minFrameSize, Rect2i::s_from(clientRect).size);
	auto* renderer    = Renderer_Dx12::s_instance();
	auto* dev		  = renderer->d3dDevice();

	_graphCmdQueue.create(dev);
	_computeCmdQueue.create(dev);

	{	// create swap chain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width                 = ax_safe_cast(frameSize.x);
		swapChainDesc.Height                = ax_safe_cast(frameSize.y);
		swapChainDesc.Format                = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount           = ax_safe_cast(_swapChainDesc.backBufferCount);
		swapChainDesc.SampleDesc.Count      = 1;
		swapChainDesc.SampleDesc.Quality    = 0;
		swapChainDesc.Scaling               = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode             = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags                 = 0; // _swapChainDesc.vsync ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		_swapChain_dx12.create(_graphCmdQueue, _hwnd, swapChainDesc);
	}

	RenderDepthType depthType = _swapChainDesc.depthBufferAttachment.depthType;
	if (depthType != RenderDepthType::None) {
		RenderPassDepthBuffer_CreateDesc depthBuf_createDesc;
		depthBuf_createDesc.name = Fmt("BackBuffer-depth");
		depthBuf_createDesc.frameSize = frameSize;
		depthBuf_createDesc.attachment = _swapChainDesc.depthBufferAttachment;
		_depthBuffer_dx12 = RenderPassDepthBuffer_Backend::s_new(AX_ALLOC_REQ, depthBuf_createDesc);
	}
	
	_createBackBuffers();
}

void RenderContext_Dx12::_createBackBuffers() {
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	auto hr = _swapChain_dx12->GetDesc1(&swapChainDesc);
	Dx12Util::throwIfError(hr);

	auto frameSize = Vec2i(swapChainDesc.Width, swapChainDesc.Height);
	
	RenderPassDepthBuffer_CreateDesc depthBuf_createDesc;
	depthBuf_createDesc.name = Fmt("BackBuffer-depth");
	depthBuf_createDesc.frameSize = frameSize;
	depthBuf_createDesc.attachment = _swapChainDesc.depthBufferAttachment;
	
	_depthBuffer_dx12 = RenderPassDepthBuffer_Backend::s_new(AX_ALLOC_REQ, depthBuf_createDesc); 

	Int bufferCount = ax_safe_cast(swapChainDesc.BufferCount);
	_backBuffers_dx12.resize(bufferCount);
	
	for (Int i = 0; i < bufferCount; ++i) {
		auto& dst = _backBuffers_dx12[i];
		if (!dst) {
			dst.newObject(AX_ALLOC_REQ);
		}
		dst->createOrUpdate(this, i, frameSize);
	}
}

void RenderContext_Dx12::BackBuffer_Dx12::createOrUpdate(RenderContext_Dx12* renderContext, Int index, Vec2i frameSize) {
	_index = index;
	auto  backBufferName  = Fmt("BackBuffer_{}-color", index);
	auto& colorBufferAttachment = renderContext->_swapChainDesc.colorBufferAttachment;
	auto& depthBufferAttachment = renderContext->_swapChainDesc.depthBufferAttachment;

	RenderPassColorBuffer_CreateDesc	colorBuf_createDesc;
	colorBuf_createDesc.name = backBufferName;
	colorBuf_createDesc.frameSize = frameSize;
	colorBuf_createDesc.backBufferRef.set(renderContext, index);
	colorBuf_createDesc.attachment.colorType = colorBufferAttachment.colorType;
	
	_colorBuf_dx12 = RenderPassColorBuffer_Backend::s_new(AX_ALLOC_REQ, colorBuf_createDesc);

	RenderPass_CreateDesc renderPass_createDesc;
	renderPass_createDesc.name = Fmt("BackBuffer_{}", index);
	renderPass_createDesc.setBackBuffer(renderContext, index);
	renderPass_createDesc.colorBufferAttachments.emplaceBack(colorBufferAttachment);
	renderPass_createDesc.depthBufferAttachment = depthBufferAttachment;
	renderPass_createDesc.frameSize   = frameSize;

	_renderPass_dx12 = RenderPass_Backend::s_new(AX_ALLOC_REQ, renderPass_createDesc);
}

void RenderContext_Dx12::BackBuffer_Dx12::releaseResources() {
	if (_colorBuf_dx12) {
		_colorBuf_dx12->releaseResources();
	}

	if (_renderPass_dx12) {
		_renderPass_dx12->releaseResources();
	}
}

void RenderContext_Dx12::onSetRenderNeeded() {
	if (_hwnd) {
		::RedrawWindow(_hwnd, nullptr, nullptr, RDW_INVALIDATE);
	}
}

void RenderContext_Dx12::onPostCreate(const CreateDesc& desc) {
	Base::onPostCreate(desc);
	if (_hwnd) {
		::ShowWindow(_hwnd, SW_SHOW);
	}
#if 0
	::SetTimer(_hwnd, kRenderTimerId, 0, nullptr);
#endif

	_createSwapChain();
}

void RenderContext_Dx12::onSetFrameSize(const Vec2i& s) {
	Base::onSetFrameSize(s);
	if (!_hwnd) return;
	::SetWindowPos(_hwnd, nullptr, 0, 0, ax_safe_cast(s.x), ax_safe_cast(s.y), 0);

	// release resource before resize swap chain
	for (auto& backBuffer : _backBuffers_dx12) {
		backBuffer->releaseResources();
	}

	Vec2i frameSize = Math::max(_minFrameSize, s);
	auto hr = _swapChain_dx12->ResizeBuffers(0, ax_safe_cast(frameSize.x), ax_safe_cast(frameSize.y), DXGI_FORMAT_UNKNOWN, 0);
	Dx12Util::throwIfError(hr);

	_createBackBuffers();
}

LRESULT WINAPI RenderContext_Dx12::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE: {
			auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			auto* thisObj = static_cast<This*>(cs->lpCreateParams);
			thisObj->_hwnd = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)thisObj);
		}break;

		case WM_DESTROY: {
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)nullptr);
		}break;

		case WM_PAINT: {
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->render();
			}
			EndPaint(hwnd, &ps);
		}break;

		case WM_TIMER: {
			if (auto* thisObj = s_getThis(hwnd)) {
				if (wParam == kRenderTimerId) {
					thisObj->render();
				}
			}
		} break;			

		default: {
			if (auto* thisObj = s_getThis(hwnd)) {
				if (thisObj->_uiEventHandler.handleEvent(hwnd, msg, wParam, lParam))
					return 0;
			}
		}break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

RenderPass_Backend* RenderContext_Dx12::onAcquireBackBufferRenderPass(RenderRequest* req) {
	if (!_swapChain_dx12) return nullptr;
	Int backBuffIndex = ax_safe_cast(_swapChain_dx12->GetCurrentBackBufferIndex());
	auto* backBuffer = _getBackBuffer(backBuffIndex);
	if (!backBuffer) return nullptr;
	return backBuffer->_renderPass_dx12;
}

void RenderContext_Dx12::onPresentSurface(RenderRequest* req_) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	if (!req) { AX_ASSERT(false); return; }

	Array<ID3D12CommandList*, 4> lists;
	lists.emplaceBack(req->_graphCmdBuf_dx12);
	
	_graphCmdQueue->ExecuteCommandLists(ax_safe_cast(lists.size()), lists.data());

	auto hr = _swapChain_dx12->Present(_swapChainDesc.vsync ? 1 : 0, 0);
	Dx12Util::throwIfError(hr);
}

void RenderContext_Dx12::_createWindow(const CreateDesc& desc) {
	auto hInstance = ::GetModuleHandle(nullptr);
	HWND parentHwnd = desc.window ? desc.window->hwnd() : nullptr;

	static const wchar_t* kClassName = L"RenderContext_Dx12";
	
	// register window class
	WNDCLASSEX wc = {};
	bool registered = (0 != ::GetClassInfoEx(hInstance, kClassName, &wc));
	if (!registered) {
		wc.cbSize			= sizeof(wc);
		wc.style			= CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= s_wndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInstance;
		wc.hIcon			= nullptr;
		wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground	= nullptr; //(HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName		= nullptr;
		wc.lpszClassName	= kClassName;
		wc.hIconSm			= nullptr;
			
		if (!::RegisterClassEx(&wc)) {
			throw Error_Undefined();
		}
	}

	auto rc = desc.windowDesc.rect;
	//--------
	DWORD dwStyle = parentHwnd ? WS_CHILD : WS_POPUP;
	DWORD dwExStyle = 0;
	_hwnd = ::CreateWindowExW(dwExStyle, kClassName, kClassName, dwStyle,
								ax_safe_cast(rc.x),
								ax_safe_cast(rc.y),
								ax_safe_cast(rc.w),
								ax_safe_cast(rc.h),
								parentHwnd, nullptr, hInstance, this);
	if (!_hwnd) {
		throw Error_Undefined();
	}

	ShowWindow(_hwnd, SW_SHOW);
}

} // namespace

#endif //AX_RENDERER_DX12
