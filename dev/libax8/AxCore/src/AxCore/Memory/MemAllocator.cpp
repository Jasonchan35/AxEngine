module;
#include "AxCore-pch.h"

module AxCore.Allocator;

namespace ax {

MemAllocator* ax_default_allocator() {
	static MemAllocator s;
	return &s;
}

void* ax_operator_new_handler(size_t sizeInBytes, const MemAllocRequest& req_) {
	if (!req_.allocator) {
		throw Error_Allocator();
	}

	MemAllocRequest req(req_.allocator, req_.srcLoc);
	req.dataSize = ax_safe_cast_Int(sizeInBytes);
	
	auto result = req.allocator->allocBytes(req);
	return result.takeOwnership();
}

void ax_operator_delete_handler(void* block, const MemAllocRequest& req) {
	// get called, if constructor throw exception during 'new'

	if (!block) return;
	if (!req.allocator) {
		AX_ASSERT(false);
		return;
	}
	ax_free(block);
}

} // namespace
