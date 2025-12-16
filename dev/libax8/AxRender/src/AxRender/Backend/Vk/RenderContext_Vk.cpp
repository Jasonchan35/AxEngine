module AxRender;

#if AX_RENDERER_VK
import :RenderContext_Vk;
import :RenderRequest;
import :Renderer_Vk;
import :Texture_Vk;
import :RenderRequest_Vk;
import :GpuBuffer_Vk;

namespace ax /*::AxRender*/ {

RenderContext_Vk_Base::RenderContext_Vk_Base(const CreateDesc& desc)
: Base(desc)
{
	_viewportIsBottomUp = true;
}

RenderContext_Vk_Base::~RenderContext_Vk_Base() {
	if (auto* renderer = Renderer_Vk::s_instance()) {
		renderer->device().waitIdle(); // wait all commandQueue is done
	}
}

void RenderContext_Vk_Base::onPostCreate(const CreateDesc& desc) {
	Base::onPostCreate(desc);

	_surface_vk.getGraphQueue(_graphQueue_vk, 0);
	_surface_vk.getPresentQueue(_presentQueue_vk, 0);

#if AX_DEBUG_NAME
	_graphQueue_vk.setDebugName("RenderContext-GraphQueue");
	_presentQueue_vk.setDebugName("RenderContext-PresentQueue");
#endif

	_createSwapChain();
}

void RenderContext_Vk_Base::_createSwapChain() {
	auto& dev = Renderer_Vk::s_instance()->device();

	auto cap = _surface_vk.getCapabilities();
	Vec2i frameSize = Math::max(_minFrameSize, AX_VkUtil::castVec2i(cap.currentExtent));

//-- renderPass and framebuffer
	VkSurfaceFormatKHR	surfaceFormat;
	surfaceFormat.format		= AX_VkUtil::getVkColorType(_swapChainDesc.colorBufferAttachment.colorType);
	surfaceFormat.colorSpace	= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

//------
//	VkPresentModeKHR	presentMode	= _swapChainDesc.vsync ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
	VkPresentModeKHR	presentMode	= _swapChainDesc.vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;

	_swapChain_vk.create(dev, _surface_vk, surfaceFormat, _swapChainDesc.backBufferCount, presentMode);

	RenderDepthType depthType = _swapChainDesc.depthBufferAttachment.depthType;
	if (depthType != RenderDepthType::None) {
		RenderPassDepthBuffer_CreateDesc depthBuf_createDesc;
		depthBuf_createDesc.name = Fmt("BackBuffer-depth");
		depthBuf_createDesc.frameSize = frameSize;
		depthBuf_createDesc.attachment.depthType = depthType;
		_depthBuf_vk = RenderPassDepthBuffer_Backend::s_new(AX_ALLOC_REQ, depthBuf_createDesc);
	}

	_createBackBuffers(dev, frameSize);
}

void RenderContext_Vk_Base::_createBackBuffers(AX_VkDevice& dev, Vec2i frameSize) {
	Array<VkImage, 8>	images;
	_swapChain_vk.getImages(images);

	Int imageCount = images.size();
	_backBuffers_vk.resize(imageCount);

	for (Int i = 0; i < imageCount; i++) {
		auto& dst = _backBuffers_vk[i];
		if (!dst) {
			dst.newObject(AX_ALLOC_REQ);
		}
		dst->createOrUpdate(this, dev, i, images[i], frameSize);
	}
}

RenderPass_Backend* RenderContext_Vk_Base::onAcquireBackBufferRenderPass(RenderRequest* req_) {
	auto* req = rttiCastCheck<RenderRequest_Vk>(req_);
	if (!req) { AX_ASSERT(false); return nullptr; }

	u32 swapChainImageIndex = UINT32_MAX;

	constexpr Int kMaxRecreate = 2;
	for(Int i = 0; i < kMaxRecreate; i++) {
		auto err = _swapChain_vk.acquireNextImage(	swapChainImageIndex, 
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

	Int backBufferIndex = ax_safe_cast(swapChainImageIndex);
	auto& backBuf = _backBuffers_vk[backBufferIndex];
	if (!backBuf) {
		AX_ASSERT(false);
		return nullptr;
	}

	return backBuf->_renderPass_vk;
}

void RenderContext_Vk_Base::onPresentSurface(RenderRequest* req_) {
	auto* req = rttiCastCheck<RenderRequest_Vk>(req_);
	if (!req) { AX_ASSERT(false); return; }

	const bool presentQueueIsSeparated = _surface_vk.isPresentQueueIsSeparated();

	AX_VkDeviceQueue::WaitSemaphores waitSem(req->_imageAcquiredSemaphore_vk, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	auto& graphSemaphore = req->_graphSemaphore_vk;

	auto* backBuf = req->backBufferRenderPass();
	if (!backBuf) { AX_ASSERT(false); return; }

	RenderPass::ColorBuffer* colorBuffer = backBuf->colorBuffers().tryGetElement(0);
	if (!colorBuffer) { AX_ASSERT(false); return; }
	
	auto* colorBuffer_vk = rttiCastCheck<RenderPassColorBuffer_Vk>(colorBuffer->buffer.ptr());
	if (!colorBuffer_vk) { AX_ASSERT(false); return; }

	auto& backBufferRef    = colorBuffer_vk->backBufferRef();
	auto* backBuffer       = _getBackBuffer(backBufferRef.index);
	auto& presentCmdBuf    = backBuffer->_presentCmdBuf_vk;
	auto& presentSemaphore = backBuffer->_presentSemaphore_vk;

	Array<VkCommandBuffer, 2>	cmdBuffers;
	cmdBuffers.append(req->_uploadCmdBuf_vk);
	cmdBuffers.append(req->_graphCmdBuf_vk);

	req->_completedFence_vk.reset();
	
	_graphQueue_vk.submit( waitSem,
						cmdBuffers,
						presentQueueIsSeparated ? graphSemaphore.handle() : presentSemaphore.handle(),
						req->_completedFence_vk.handle());

	if (presentQueueIsSeparated) {
		// If we are using separate queues, change image ownership to the
		// present queue before presenting, waiting for the draw complete
		// semaphore and signaling the ownership released semaphore when finished

		presentCmdBuf->beginCommand();
		presentCmdBuf->pipelineBarrier(	_surface_vk.graphQueueFamilyIndex(), 
										_surface_vk.presentQueueFamilyIndex(),
										colorBuffer_vk->_image);
		presentCmdBuf->endCommand();

		_presentQueue_vk.submit(AX_VkDeviceQueue::WaitSemaphores(graphSemaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT),
								presentCmdBuf.handle(),
								presentSemaphore.handle());
	}

	_graphQueue_vk.present(presentSemaphore, _swapChain_vk, backBufferRef.index);
}

void RenderContext_Vk_Base::BackBuffer_Vk::createOrUpdate(
	RenderContext_Vk_Base* renderContext,
	AX_VkDevice&	dev,
	Int				index,
	VkImage&		vkImage,
	Vec2i			frameSize
) {
	_index = index;
	_vkImage = vkImage;
	
	auto backBufferName = Fmt("BackBuffer_{}-color", index);
	dev.setObjectDebugName(vkImage, backBufferName);

	auto& colorBufferAttachment = renderContext->_swapChainDesc.colorBufferAttachment;
	auto& depthBufferAttachment = renderContext->_swapChainDesc.depthBufferAttachment;

	RenderPassColorBuffer_CreateDesc	colorBuf_createDesc;
	colorBuf_createDesc.name      = backBufferName;
	colorBuf_createDesc.frameSize = frameSize;
	colorBuf_createDesc.attachment = colorBufferAttachment;
	colorBuf_createDesc.backBufferRef.set(renderContext, index);

	_colorBuf_vk = RenderPassColorBuffer_Backend::s_new(AX_ALLOC_REQ, colorBuf_createDesc);

	auto& surface = renderContext->_surface_vk;

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

	RenderPass_CreateDesc renderPass_createDesc;
	renderPass_createDesc.name = Fmt("BackBuffer_{}", index);
	renderPass_createDesc.setBackBuffer(renderContext, index);
	renderPass_createDesc.colorBufferAttachments.emplaceBack(colorBufferAttachment);
	renderPass_createDesc.depthBufferAttachment = depthBufferAttachment;
	renderPass_createDesc.frameSize   = frameSize;

	_renderPass_vk = RenderPass_Backend::s_new(AX_ALLOC_REQ, renderPass_createDesc);
}

#if AX_NATIVE_UI_WIN32


RenderContext_Vk_Win32::RenderContext_Vk_Win32(const CreateDesc& desc) 
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
	DWORD dwStyle   = parentHwnd ? WS_CHILD : WS_POPUP;
	DWORD dwExStyle = 0;
	auto  rc        = desc.windowDesc.rect;

	_hwnd = ::CreateWindowExW(dwExStyle,
	                          kClassName,
	                          kClassName,
	                          dwStyle,
	                          ax_safe_cast(rc.x),
	                          ax_safe_cast(rc.y),
	                          ax_safe_cast(rc.w),
	                          ax_safe_cast(rc.h),
	                          parentHwnd,
	                          nullptr,
	                          hInstance,
	                          this);
	if (!_hwnd) {
		AX_LOG_WIN32_LAST_ERROR("CreateWindow");
		throw Error_Undefined();
	}

	auto& dev = Renderer_Vk::s_instance()->device();
	_surface_vk.create_Win32(dev, hInstance, _hwnd);
}

void RenderContext_Vk_Win32::onPostCreate(const CreateDesc& desc) {
	Base::onPostCreate(desc);
	if (_hwnd) {
		::ShowWindow(_hwnd, SW_SHOW);
	}
#if 0
	::SetTimer(_hwnd, kRenderTimerId, 0, nullptr);
#endif
}

LRESULT WINAPI RenderContext_Vk_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

void RenderContext_Vk_Win32::onSetFrameSize(const Vec2i& s) {
	Base::onSetFrameSize(s);

	if (!_hwnd) return;
	::SetWindowPos(_hwnd, nullptr, 0, 0, ax_safe_cast(s.x), ax_safe_cast(s.y), 0);
}

void RenderContext_Vk_Win32::onSetRenderNeeded() {
	if (_hwnd) {
		InvalidateRect(_hwnd, nullptr, false);
	}
}

RenderContext_Vk_Win32::~RenderContext_Vk_Win32() {
	if (_hwnd) {
		::DestroyWindow(_hwnd);
		_hwnd = nullptr;
	}
}

#endif

} // namespace 
#endif // AX_RENDERER_VK