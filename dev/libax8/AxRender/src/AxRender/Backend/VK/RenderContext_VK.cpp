module AxRender;

#if AX_RENDERER_VK
import :RenderContext_VK;
import :RenderRequest;
import :Renderer_VK;
import :Texture_VK;
import :RenderRequest_VK;
import :GpuBuffer_VK;

namespace ax::AxRender {

RenderContext_VK_Base::RenderContext_VK_Base(const CreateDesc& desc)
: Base(desc)
{
	_viewportIsBottomUp = true;
}

RenderContext_VK_Base::~RenderContext_VK_Base() {
	if (auto* renderer = Renderer_VK::s_instance()) {
		renderer->device().waitIdle(); // wait all commandQueue is done
	}
}

void RenderContext_VK_Base::onPostCreate(const CreateDesc& desc) {
	Base::onPostCreate(desc);

	_surface.getGraphQueue(_graphQueue, 0);
	_surface.getPresentQueue(_presentQueue, 0);

#if AX_DEBUG_NAME
	_graphQueue.setDebugName("RencerContext-GraphQueue");
	_presentQueue.setDebugName("RencerContext-PresentQueue");
#endif

	_createSwapChain();
}

void RenderContext_VK_Base::_createSwapChain() {

	auto& dev = Renderer_VK::s_instance()->device();

	auto cap = _surface.getCapabilities();
	auto frameSize_vk = cap.currentExtent;

	Vec2i frameSize(frameSize_vk.width, frameSize_vk.height);

	if (frameSize.x <= 0 || frameSize.y <= 0)
		return;

//-- renderPass and framebuffer
	VkSurfaceFormatKHR	surfaceFormat;
	surfaceFormat.format		= AX_VkUtil::getVkColorType(_colorBufferDesc.colorType);
	surfaceFormat.colorSpace	= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	VkFormat depthFormat	= AX_VkUtil::getVkDepthType(_depthBufferDesc.depthType);

//------
//	VkPresentModeKHR	presentMode	= _vsync ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
	VkPresentModeKHR	presentMode	= _vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;

	_swapChain.create(dev, _surface, surfaceFormat, _requestBackBufferCount, presentMode);

	bool hasDepth = depthFormat != VK_FORMAT_UNDEFINED;
	if (hasDepth) {
		RenderDepthBuffer_CreateDesc depthBufDesc;
		depthBufDesc.name = Fmt("BackBuffer-depth");
		depthBufDesc.depthType = _depthBufferDesc.depthType;
		depthBufDesc.frameSize = frameSize;

		_depthBuf = RenderDepthBuffer_Backend::s_new(AX_ALLOC_REQ, depthBufDesc);
	}

	{	// init Back Buffers
		_swapChain.getImages(_backBufImages);
		Int resultBackBufferCount = _backBufImages.size();
		
		_backBuffers.resize(resultBackBufferCount);

		for (Int i = 0; i < resultBackBufferCount; i++) {
			auto& dst = _backBuffers[i];
			if (!dst) {
				dst.newObject(AX_ALLOC_REQ);
			}
			dst->createOrUpdate(this, dev, i, frameSize);
		}
	}
}

RenderPass_Backend* RenderContext_VK_Base::onAcquireBackBufferRenderPass(RenderRequest* req_) {
	auto* req = rttiCastCheck<RenderRequest_VK>(req_);
	if (!req) { AX_ASSERT(false); return nullptr; }

	u32 swapChainImageIndex = UINT32_MAX;

	constexpr Int kMaxRecreate = 2;
	for(Int i = 0; i < kMaxRecreate; i++) {
		auto err = _swapChain.acquireNextImage(	swapChainImageIndex, 
												req->_imageAcquiredSemaphore_vk, 
												VK_NULL_HANDLE);
		if (err == VK_SUCCESS) {
			break;
		} else if (err == VK_ERROR_OUT_OF_DATE_KHR) {
			// demo->swapchain is out of date (e.g. the window was resized) and
			// must be recreated:
			_createSwapChain();
			continue;

		} else if (err == VK_SUBOPTIMAL_KHR) {
			// demo->swapchain is not as optimal as it could be, but the platform's
			// presentation engine will still present the image correctly.
			break;
		} else {
			AX_VkUtil::throwIfError(err);
		}
	}

	Int backBufferIndex = SafeCast(swapChainImageIndex);
	auto* backBuf = _getBackBuffer(backBufferIndex);
	if (!backBuf) {
		AX_ASSERT(false);
		return nullptr;
	}

	return rttiCastCheck<RenderPass_Backend>(backBuf->_renderPass.ptr());
}

void RenderContext_VK_Base::onPresentSurface(RenderRequest* req_) {
	auto* req = rttiCastCheck<RenderRequest_VK>(req_);
	if (!req) { AX_ASSERT(false); return; }

	const bool presentQueueIsSeparated = _surface.isPresentQueueIsSeparated();

	AX_VkDeviceQueue::WaitSemaphores waitSem(req->_imageAcquiredSemaphore_vk, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	auto& graphSemaphore = req->_graphSemaphore_vk;

	auto* backBuf = req->backBufferRenderPass();
	if (!backBuf) { AX_ASSERT(false); return; }

	auto* colorBuffer	= rttiCastCheck<RenderColorBuffer_VK>(backBuf->colorBuffer(0));
	if (!colorBuffer) { AX_ASSERT(false); return; }

	auto& backBufferRef = colorBuffer->backBufferRef();
	auto* backBuffer	   = _getBackBuffer(backBufferRef.index);
	auto& presentCmdBuf	   = backBuffer->_presentCmdBuf_vk;
	auto& presentSemaphore = backBuffer->_presentSemaphore_vk;


	Array<VkCommandBuffer, 2>	cmdBuffers;
	cmdBuffers.append(req->_uploadCmdBuf_vk);
	cmdBuffers.append(req->_graphCmdBuf_vk);

	req->_completedFence_vk.reset();
	
	_graphQueue.submit( waitSem,
						cmdBuffers,
						presentQueueIsSeparated ? graphSemaphore.handle() : presentSemaphore.handle(),
						req->_completedFence_vk.handle());

	if (presentQueueIsSeparated) {
		// If we are using separate queues, change image ownership to the
		// present queue before presenting, waiting for the draw complete
		// semaphore and signaling the ownership released semaphore when finished

		presentCmdBuf->beginCommand();
		presentCmdBuf->pipelineBarrier(	_surface.graphQueueFamilyIndex(), 
										_surface.presentQueueFamilyIndex(),
										colorBuffer->_image);
		presentCmdBuf->endCommand();

		_presentQueue.submit(	AX_VkDeviceQueue::WaitSemaphores(graphSemaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT),
								presentCmdBuf.handle(),
								presentSemaphore.handle());
	}

	_graphQueue.present(presentSemaphore, _swapChain, backBufferRef.index);
}

void RenderContext_VK_Base::BackBuffer_VK::createOrUpdate(
	RenderContext_VK_Base* renderContext,
	AX_VkDevice&           dev,
	Int                    index,
	Vec2i                  frameSize
) {
	auto backBufferName = Fmt("BackBuffer_{}-color", index);
	dev.setObjectDebugName(renderContext->_backBufImages[index], backBufferName);

	auto& colorBufferDesc = renderContext->_colorBufferDesc;
	auto& depthBufferDesc = renderContext->_depthBufferDesc;

	RenderColorBuffer_CreateDesc	colorBufDesc;
	colorBufDesc.name = backBufferName;
	colorBufDesc.setBackBuffer(	renderContext, index, 
								colorBufferDesc.colorType, 
								frameSize);

	_colorBuf = RenderColorBuffer_Backend::s_new(AX_ALLOC_REQ, colorBufDesc);

	auto& surface = renderContext->_surface;

	if (!_presentSemaphore_vk) {
		_presentSemaphore_vk.create(dev);
	}
	
	if (!_presentCmdBuf_vk) {
		_presentCmdBuf_vk.create(dev, surface.presentQueueFamilyIndex());

#if AX_DEBUG_NAME
		_presentSemaphore_vk.setDebugName(Fmt("RenderReq_{}-presentSemaphore", index));
			_presentCmdBuf_vk.setDebugName(Fmt("RenderReq_{}-presentCmdBuf",   index));
#endif
	}

	RenderPass_CreateDesc	renderPassDesc;
	renderPassDesc.name = Fmt("BackBuffer_{}", index);
	renderPassDesc.setBackBuffer(renderContext, index);
	renderPassDesc.colorBuffers.emplaceBack(colorBufferDesc);
	renderPassDesc.depthBuffer = depthBufferDesc;
	renderPassDesc.frameSize = frameSize;

	_renderPass = RenderPass_Backend::s_new(AX_ALLOC_REQ, renderPassDesc);
}

#if AX_NATIVE_UI_WIN32


RenderContext_VK_Win32::RenderContext_VK_Win32(const CreateDesc& desc) 
: Base(desc)
, _uiEventHandler(this)
{
	static const wchar_t* kClassName = L"RenderContext_VK_Win32";

	auto hInstance = ::GetModuleHandle(nullptr);

	// register window class
	WNDCLASSEX wc = {};
	bool registered = (0 != ::GetClassInfoEx(hInstance, kClassName, &wc));
	if (!registered) {
		wc.cbSize			= sizeof(wc);
		wc.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= s_wndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInstance;
		wc.hIcon			= nullptr;
		wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground	= nullptr; // (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName		= nullptr;
		wc.lpszClassName	= kClassName;
		wc.hIconSm			= nullptr;
			
		if (!::RegisterClassEx(&wc)) {
			throw Error_Undefined();
		}
	}

	HWND parentHwnd = desc.window ? desc.window->hwnd() : nullptr;

	// create real context
	DWORD dwStyle = parentHwnd ? WS_CHILD : WS_POPUP;
	DWORD dwExStyle = 0;
	_hwnd = ::CreateWindowExW(dwExStyle, kClassName, kClassName, dwStyle,
								0, 0, 8, 8,
								parentHwnd, nullptr, hInstance, this);
	if (!_hwnd) {
		AX_LOG_WIN32_LAST_ERROR("CreateWindow");
		throw Error_Undefined();
	}

	auto& dev = Renderer_VK::s_instance()->device();
	_surface.create_Win32(dev, hInstance, _hwnd);
}

void RenderContext_VK_Win32::onPostCreate(const CreateDesc& desc) {
	Base::onPostCreate(desc);
	if (_hwnd) {
		::ShowWindow(_hwnd, SW_SHOW);
	}
#if 0
	::SetTimer(_hwnd, kRenderTimerId, 0, nullptr);
#endif
}

LRESULT WINAPI RenderContext_VK_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE: {
			auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			auto* thisObj = static_cast<This*>(cs->lpCreateParams);
			thisObj->_hwnd = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisObj));
		}break;

		case WM_DESTROY: {
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
		}break;

		case WM_PAINT: {
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->render();
			}
			EndPaint(hwnd, &ps);
		} break;

		case WM_TIMER: {
			if (auto* thisObj = s_getThis(hwnd)) {
				if (wParam == RenderContext_VK_Win32::kRenderTimerId) {
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

void RenderContext_VK_Win32::onSetFrameSize(const Vec2i& s) {
	Base::onSetFrameSize(s);

	if (!_hwnd) return;
	SetWindowPos(_hwnd, nullptr, 0, 0, SafeCast(s.x), SafeCast(s.y), 0);
}

void RenderContext_VK_Win32::onSetRenderNeeded() {
	if (_hwnd) {
		InvalidateRect(_hwnd, nullptr, false);
	}
}

RenderContext_VK_Win32::~RenderContext_VK_Win32() {
	if (_hwnd) {
		::DestroyWindow(_hwnd);
		_hwnd = nullptr;
	}
}

Vec2f RenderContext_VK_Win32::worldToLocalPos(const Vec2f& pt) {
	return NativeUI_Win32::s_worldToLocalPos(_hwnd, pt);
}

Vec2f RenderContext_VK_Win32::localToWorldPos(const Vec2f& pt) {
	return NativeUI_Win32::s_worldToLocalPos(_hwnd, pt);
}


#endif

} // namespace 
#endif // AX_RENDERER_VK