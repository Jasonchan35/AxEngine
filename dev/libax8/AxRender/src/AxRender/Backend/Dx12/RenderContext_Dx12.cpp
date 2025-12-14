module AxRender;
import :RenderContext_Dx12;

#if AX_RENDERER_DX12

namespace ax {

const wchar_t* RenderContext_Dx12_Win32_ClassName = L"RenderContext_Dx12";

RenderContext_Dx12::RenderContext_Dx12(Renderer_Dx12* renderer, CreateDesc& desc)
	: Base(renderer, desc)
{
	_createWindow(desc);

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
		swapChainDesc.BufferCount = kFrameBufferCount;
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

		_setFrameIndex(_swapChain->GetCurrentBackBufferIndex());
	}

	_createRenderTargetView();
}

Renderer_Dx12* RenderContext_Dx12::renderer() {
	return static_cast<Renderer_Dx12*>(_renderer);
}

void RenderContext_Dx12::_createRenderTargetView() {
	_releaseRenderTargetView();

	_renderTargetDescHeap.init(kFrameBufferCount);
	_depthStencilDescHeap.init(kFrameBufferCount);

	// Create a RTV for each frame.
	for (UINT i = 0; i < kFrameBufferCount; i++) {
		auto* frame = &_frameArray[i];
		frame->index = _swapChain->GetCurrentBackBufferIndex();

		frame->_renderTargetResource.createFromSwapChain(_swapChain, i);

		DXGI_SWAP_CHAIN_DESC desc;
		auto hr = _swapChain->GetDesc(&desc);
		Dx12Util::throwIfError(hr);

		Vec2i bufferSize(desc.BufferDesc.Width, desc.BufferDesc.Height);
		frame->_depthStencilBufferResource.create(bufferSize);

		frame->renderTarget       = _renderTargetDescHeap.createView(i, frame->_renderTargetResource);
		frame->depthStencilBuffer = _depthStencilDescHeap.createView(i, frame->_depthStencilBufferResource);
	}
}

void RenderContext_Dx12::_releaseRenderTargetView() {
	for (UINT i = 0; i < kFrameBufferCount; i++) {
		auto* f = &_frameArray[i];
		f->_renderTargetResource.destroy();
		f->_depthStencilBufferResource.destroy();
	}
}


void RenderContext_Dx12::cmdSwapBuffers(CommandDispatcher_Dx12& dispatcher) {
	{	// Indicate that the back buffer will now be used to present.
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = _frame->_renderTargetResource.d3dResource();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		dispatcher.cmdList->ResourceBarrier(1, &barrier);
	}
	dispatcher._execCmdList();

	{
		auto* fence = rttiCastCheck<axDX12Fence>(dispatcher.renderRequest->completedFence());
		fence->addToGpu(dispatcher.cmdQueue);
	}

	auto hr = _swapChain->Present(_renderer->vsync() ? 1 : 0, 0);
	Dx12Util::throwIfError(hr);
}

void RenderContext_Dx12::onDispatchCommands(RenderRequest& req_) {
	_setFrameIndex(_swapChain->GetCurrentBackBufferIndex());

	auto* req = rttiCastCheck<axDX12RenderRequest>(&req_);

	CommandDispatcher_Dx12 dispatcher(this, req);
	dispatcher.cmdQueue.ref(_cmdQueue);
	dispatcher.computeCmdQueue.ref(_computeCmdQueue);
	dispatcher.renderTarget = _frame->renderTarget;
	dispatcher.depthStencilBuffer = _frame->depthStencilBuffer;

	// Set necessary state.
	//_cmdList->SetGraphicsRootSignature(m_rootSignature.Get());
	//_cmdList->RSSetViewports(1, &m_viewport);
	//_cmdList->RSSetScissorRects(1, &m_scissorRect);

	{	// Indicate that the back buffer will be used as a render target.
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = _frame->_renderTargetResource.d3dResource();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		dispatcher.cmdList->ResourceBarrier(1, &barrier);
	}
	dispatcher.cmdList->OMSetRenderTargets(1, &_frame->renderTarget.cpu, FALSE, &_frame->depthStencilBuffer.cpu);


#if AX_RENDERER_DX12_DXR
	dispatcher.rayTracing = rttiCastCheck<axDX12RayTracing>(renderer()->rayTracing());
#endif


	dispatcher.dispatch();
}

void RenderContext_Dx12::onPreDestroy() {
}

void RenderContext_Dx12::onSetNeedToRender() {
	if (_hwnd) {
		::RedrawWindow(_hwnd, nullptr, nullptr, RDW_INVALIDATE);
	}
}

RenderContext_Dx12::~RenderContext_Dx12() {
}

void RenderContext_Dx12::onSetNativeViewRect(const Rect2f& rect) {
	::MoveWindow(_hwnd, int(rect.x), int(rect.y), int(rect.w), int(rect.h), false);

	_releaseRenderTargetView();

	auto width  = static_cast<UINT>(ax_max(8.0f, rect.w));
	auto height = static_cast<UINT>(ax_max(8.0f, rect.h));
	auto hr = _swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	Dx12Util::throwIfError(hr);

	_createRenderTargetView();

	Base::onSetNativeViewRect(rect);
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
				if (thisObj->_eventHandler) {
					thisObj->_eventHandler->render(thisObj);
				}
			}
			EndPaint(hwnd, &ps);
			return 0;
		}break;

		default: {
			if (auto* thisObj = s_getThis(hwnd)) {
				return thisObj->_window->_handleWin32Event(hwnd, msg, wParam, lParam);
			}
		}break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

void RenderContext_Dx12::_createWindow(CreateDesc& desc) {
	auto hInstance = ::GetModuleHandle(nullptr);
	HWND parentHwnd = desc.window ? desc.window->hwnd() : nullptr;

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

void RenderContext_Dx12::_setFrameIndex(UINT i) {
	_frameIndex = i;
	_frame = &_frameArray[i];
}

#endif //AX_RENDERER_DX12
