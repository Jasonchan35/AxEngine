module AxRender;
import :RenderContext_Dx12;

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
	auto  vsync       = _swapChainDesc.vsync;
	
	auto* renderer    = Renderer_Dx12::s_instance();
	auto* d3dDevice   = renderer->d3dDevice();
	auto* dxgiFactory = renderer->dxgiFactory();

	HRESULT hr;

	{	// create command queue
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type		= D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority	= D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.Flags		= D3D12_COMMAND_QUEUE_FLAG_NONE; // D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT
		queueDesc.NodeMask	= 0;

		hr = d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_cmdQueue.ptrForInit()));
		Dx12Util::throwIfError(hr);

		queueDesc.Type		= D3D12_COMMAND_LIST_TYPE_COMPUTE;
		hr = d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_computeCmdQueue.ptrForInit()));
		Dx12Util::throwIfError(hr);
	}
	
	
	{	// create swap chain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width                 = SafeCast(frameSize.x);
		swapChainDesc.Height                = SafeCast(frameSize.y);
		swapChainDesc.Format                = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount           = SafeCast(_swapChainDesc.backBufferCount);
		swapChainDesc.SampleDesc.Count      = 1;
		swapChainDesc.SampleDesc.Quality    = 0;
		swapChainDesc.Scaling               = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode             = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags                 = vsync ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		ComPtr<IDXGISwapChain1> swapChain1;
		hr = dxgiFactory->CreateSwapChainForHwnd(_cmdQueue, _hwnd, &swapChainDesc, nullptr, nullptr, swapChain1.ptrForInit());
		Dx12Util::throwIfError(hr);

		hr = swapChain1->QueryInterface(IID_PPV_ARGS(_swapChain_dx12.ptrForInit()));
		Dx12Util::throwIfError(hr);
	}

	RenderDepthType depthType = _swapChainDesc.depthBuffer.depthType;
	if (depthType != RenderDepthType::None) {
		RenderTargetDepthBuffer_CreateDesc depthBuf_createDesc;
		depthBuf_createDesc.name = Fmt("BackBuffer-depth");
		depthBuf_createDesc.depthType = depthType;
		depthBuf_createDesc.frameSize = frameSize;
		_depthBuffer_dx12 = RenderTargetDepthBuffer_Backend::s_new(AX_ALLOC_REQ, depthBuf_createDesc);
	}
	
	_createBackBuffers();
}

void RenderContext_Dx12::_createBackBuffers() {
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	auto hr = _swapChain_dx12->GetDesc1(&swapChainDesc);
	Dx12Util::throwIfError(hr);

	Int bufferCount = SafeCast(swapChainDesc.BufferCount);
	_backBuffers_dx12.resize(bufferCount);
	
	auto frameSize = Vec2i(swapChainDesc.Width, swapChainDesc.Height);
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
	auto& colorBufferDesc = renderContext->_swapChainDesc.colorBuffer;
	auto& depthBufferDesc = renderContext->_swapChainDesc.depthBuffer;

	RenderTargetColorBuffer_CreateDesc	colorBuf_createDesc;
	colorBuf_createDesc.name = backBufferName;
	colorBuf_createDesc.setBackBuffer(	renderContext, index, 
										colorBufferDesc.colorType, 
										frameSize);
	
	_colorBuf_dx12 = RenderTargetColorBuffer_Backend::s_new(AX_ALLOC_REQ, colorBuf_createDesc);

	RenderPass_CreateDesc renderPass_createDesc;
	renderPass_createDesc.name = Fmt("BackBuffer_{}", index);
	renderPass_createDesc.setBackBuffer(renderContext, index);
	renderPass_createDesc.colorBuffers.emplaceBack(colorBufferDesc);
	renderPass_createDesc.depthBuffer = depthBufferDesc;
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
	::SetWindowPos(_hwnd, nullptr, 0, 0, SafeCast(s.x), SafeCast(s.y), 0);

	// release resource before resize swap chain
	for (auto& backBuffer : _backBuffers_dx12) {
		backBuffer->releaseResources();
	}

	Vec2i frameSize = Math::max(_minFrameSize, s);
	auto hr = _swapChain_dx12->ResizeBuffers(0, SafeCast(frameSize.x), SafeCast(frameSize.y), DXGI_FORMAT_UNKNOWN, 0);
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
	Int backBuffIndex = SafeCast(_swapChain_dx12->GetCurrentBackBufferIndex());
	auto* backBuffer = _getBackBuffer(backBuffIndex);
	if (!backBuffer) return nullptr;
	return backBuffer->_renderPass_dx12;
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
								SafeCast(rc.x),
								SafeCast(rc.y),
								SafeCast(rc.w),
								SafeCast(rc.h),
								parentHwnd, nullptr, hInstance, this);
	if (!_hwnd) {
		throw Error_Undefined();
	}

	ShowWindow(_hwnd, SW_SHOW);
}

} // namespace

#endif //AX_RENDERER_DX12
