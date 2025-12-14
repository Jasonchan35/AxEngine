module;

module AxRender;

#if AX_RENDERER_VK
import :AX_VkUtil;

namespace ax /*::AxRender*/ {

AX_VkAllocatorCallbacks* AX_VkUtil::allocCallbacks() {
#if 0
	static AX_VkAllocatorCallbacks s;
	return &s;
#else
	return nullptr;
#endif
}

bool AX_VkUtil::formatHasDepth(VkFormat f) {
	switch (f) {
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_X8_D24_UNORM_PACK32:
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
		default: return false;
	}
}

bool AX_VkUtil::formatHasStencil(VkFormat f) {
	switch (f) {
		case VK_FORMAT_S8_UINT:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
		default: return false;
	}
}

AX_VkAllocatorCallbacks::AX_VkAllocatorCallbacks() {
	_allocator = ax_default_allocator();

	pUserData				= this;
	pfnAllocation			= s_alloc;
	pfnReallocation			= s_realloc;
	pfnFree					= s_free;
	pfnInternalAllocation	= s_allocNotification;
	pfnInternalFree			= s_freeNotification;
}

void* AX_VkAllocatorCallbacks::s_alloc(
	void* pUserData_, size_t sizeInBytes, size_t alignment,
	VkSystemAllocationScope allocationScope
) {
	auto* p = s_getThis(pUserData_);
	if (!p) return nullptr;
	if (!p->_allocator) return nullptr;

	MemAllocRequest req(p->_allocator);
	req.dataSize = SafeCast(sizeInBytes);
	req.alignment = alignment;
	
	auto result = p->_allocator->allocBytes(req);
	return result.takeOwnership();
}

void* AX_VkAllocatorCallbacks::s_realloc(
	void* pUserData_, void* pOriginal, size_t size, size_t alignment,
	VkSystemAllocationScope allocationScope
) {
	return s_alloc(pUserData_, size, alignment, allocationScope);
}

void AX_VkAllocatorCallbacks::s_free(void* pUserData_, void* pMemory) {
	if (pMemory) {
		ax_free(pMemory);
	}
}

void AX_VkAllocatorCallbacks::s_allocNotification(
	void* pUserData_, size_t size, VkInternalAllocationType allocationType,
	VkSystemAllocationScope allocationScope
) {}

void AX_VkAllocatorCallbacks::s_freeNotification(
	void* pUserData_, size_t size, VkInternalAllocationType allocationType,
	VkSystemAllocationScope allocationScope
) {}

} // namespace ax /*::AxRender*/
#endif // #if AX_RENDERER_VK