module AxRender;
import :RenderContext_Dx12;

#if AX_RENDERER_DX12

namespace ax {
const wchar_t* RenderContext_Dx12_Win32_ClassName = L"RenderContext_Dx12";

RenderContext_Dx12::RenderContext_Dx12(const CreateDesc& desc)
	: Base(desc)
	, _uiEventHandler(this)
{
	_createWindow(desc);

	auto* renderer = Renderer_Dx12::s_instance();
	auto* d3dDevice = renderer->d3dDevice();
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
		swapChainDesc.Width  = 8;
		swapChainDesc.Height = 8;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = kBackBufferCount;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING <-- for vsync ?

		ComPtr<IDXGISwapChain1> swapChain1;
		hr = dxgiFactory->CreateSwapChainForHwnd(_cmdQueue, _hwnd, &swapChainDesc, nullptr, nullptr, swapChain1.ptrForInit());
		Dx12Util::throwIfError(hr);

		hr = swapChain1->QueryInterface(IID_PPV_ARGS(_swapChain.ptrForInit()));
		Dx12Util::throwIfError(hr);

		_setCurrentBackBufferIndex(_swapChain->GetCurrentBackBufferIndex());
	}

	_createRenderTargetView();
}

void RenderContext_Dx12::_createRenderTargetView() {
	_releaseRenderTargetView();

	_renderTargetDescHeap.init(kBackBufferCount);
	_depthStencilDescHeap.init(kBackBufferCount);

	// Create a RTV for each frame.
	for (UINT i = 0; i < kBackBufferCount; i++) {
		auto* backBuffer = &_backBuffers[i];
		backBuffer->index = _swapChain->GetCurrentBackBufferIndex();

		backBuffer->_renderTargetResource.createFromSwapChain(_swapChain, i);

		DXGI_SWAP_CHAIN_DESC desc;
		auto hr = _swapChain->GetDesc(&desc);
		Dx12Util::throwIfError(hr);

		Vec2i bufferSize(desc.BufferDesc.Width, desc.BufferDesc.Height);
		backBuffer->_depthStencilBufferResource.create(bufferSize);

		backBuffer->renderTarget       = _renderTargetDescHeap.createView(i, backBuffer->_renderTargetResource);
		backBuffer->depthStencilBuffer = _depthStencilDescHeap.createView(i, backBuffer->_depthStencilBufferResource);
	}
}

void RenderContext_Dx12::_releaseRenderTargetView() {
	for (UINT i = 0; i < kBackBufferCount; i++) {
		auto* f = &_backBuffers[i];
		f->_renderTargetResource.destroy();
		f->_depthStencilBufferResource.destroy();
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
}

void RenderContext_Dx12::onSetFrameSize(const Vec2i& s) {
	Base::onSetFrameSize(s);
	if (!_hwnd) return;
	SetWindowPos(_hwnd, nullptr, 0, 0, SafeCast(s.x), SafeCast(s.y), 0);
	
	_releaseRenderTargetView();

	auto width  = SafeCast(Math::max(Int(8), s.x));
	auto height = SafeCast(Math::max(Int(8), s.y));
	auto hr = _swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	Dx12Util::throwIfError(hr);

	_createRenderTargetView();
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

void RenderContext_Dx12::_createWindow(const CreateDesc& desc) {
	auto hInstance = ::GetModuleHandle(nullptr);
	HWND parentHwnd = desc.window ? desc.window->hwnd() : nullptr;

	auto* kClassName = RenderContext_Dx12_Win32_ClassName;
	
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

	//--------
	DWORD dwStyle = parentHwnd ? WS_CHILD : WS_POPUP;
	DWORD dwExStyle = 0;
	_hwnd = ::CreateWindowExW(dwExStyle, kClassName, kClassName, dwStyle,
								0, 0, 0, 0,
								parentHwnd, nullptr, hInstance, this);
	if (!_hwnd) {
		throw Error_Undefined();
	}

	ShowWindow(_hwnd, SW_SHOW);
}

void RenderContext_Dx12::_setCurrentBackBufferIndex(UINT i) {
	_currentBackBufferIndex = i;
	_currentBackBuffer = &_backBuffers[i];
}

} // namespace

#endif //AX_RENDERER_DX12
