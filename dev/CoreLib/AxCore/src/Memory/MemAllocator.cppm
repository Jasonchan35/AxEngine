module;
#include "AxBase.h"

export module AxCore.Allocator;

import AxCore.Math;
import AxCore.MemoryUtil;

#include "AxBase.h"
export import AxCore.BasicType;

export namespace ax {

class MemAllocator;

AX_INLINE void ax_free(void* p) {
	std::cout << std::format("ax_free data={}\n", p);
	if (p) ::_aligned_free(p);
}

AX_INLINE void* ax_malloc(Int size, Int alignment) {
	std::cout << std::format("ax_malloc size={} alignment={}\n", size, alignment);
	return _aligned_malloc(size, alignment);
}

class MemAllocRequest : public NonCopyable {
public:
	MemAllocRequest(MemAllocator* allocator_, const SrcLoc& srcLoc_ = SrcLoc()) : allocator(allocator_), srcLoc(srcLoc_) {}
	MemAllocator*	allocator = nullptr;
	Int				dataSize = 0;
	Int				alignment = ax_sizeof<void*>;
	const SrcLoc&	srcLoc;
};

template<class T>
class MemAllocResult : public NonCopyable {
public:
	class MemAllocator* allocator = nullptr;
	T*  data	= nullptr;
	Int size	= 0;

	constexpr MemAllocResult() = default;
	constexpr MemAllocResult(const MemAllocResult&) = delete;

	constexpr MemAllocResult(MemAllocResult&& r) noexcept
		: allocator(r.allocator), data(r.data)
	{
		r.data = nullptr;
		r.size = 0;
	}

	constexpr MemAllocResult(MemAllocator* allocator_, T* data_, Int size_) noexcept
		: allocator(allocator_)
		, data(data_)
		, size(size_) {
	}
	
	constexpr ~MemAllocResult() noexcept { dealloc(); }
	
	constexpr void detach() noexcept { data = nullptr; allocator = nullptr; size = 0; }
	constexpr void dealloc();
};

class MemAllocator {
public:
	
	template<class T>
	MemAllocResult<T> allocArray(Int reqSize, Int alignment = ax_alignof<T>, const SrcLoc srcLoc = SrcLoc()) {
		constexpr Int kMinByteSize = 64;
		reqSize = Math::max(reqSize, kMinByteSize / ax_sizeof<T>);
		reqSize = Math::nextPow2_half(reqSize);

		MemAllocRequest req(nullptr, srcLoc);
		req.dataSize = reqSize * ax_sizeof<T>;
		req.alignment = alignment;
		
		auto block = onAlloc(req);
		T* data = reinterpret_cast<T*>(block.data);
		block.detach();
		return MemAllocResult<T>(this, data, reqSize);
	}

	MemAllocResult<u8> allocBytes(const MemAllocRequest & req) { return onAlloc(req); }
	
	void dealloc(void* p) { return onDealloc(p); }
	
protected:
	virtual MemAllocResult<u8> onAlloc(const MemAllocRequest & req) {
		void* data = ax_malloc(req.dataSize, req.alignment);
		std::cout << std::format("onAlloc size={} data={}\n", req.dataSize, data);
		return MemAllocResult<u8>(this, reinterpret_cast<u8*>(data), req.dataSize);
	}

	virtual void onDealloc(void* data) {
		if (!data) return;
		std::cout << std::format("onDealloc data={}\n", data);
		ax_free(data);
	}
};

template<class T> inline
constexpr void MemAllocResult<T>::dealloc() {
	if (!data || !allocator) return;
	allocator->dealloc(data);
	data = nullptr;
	size = 0;
}

MemAllocator*	ax_default_allocator();
MemAllocator*	ax_current_allocator() { return ax_default_allocator(); }


struct AxNoDelete {
	template<class T> static void deleteObject(T*) {}
	template<class T> static void deleteArray( T*) {}
};

struct AxDelete {
	template<class T>
	static void deleteObject(T* p) {
		// if (!p) return;
		// auto* block = AllocBlockBase::s_fromMem(p);
		// auto  dataSize = block->data().size();
		// if (dataSize < AX_SIZEOF(T)) { AX_ASSERT(false); }
		// MemoryUtil::destructor(p, 1);
		ax_free(p);
	}

	template<class T>
	static void deleteArray(T* p) {
		// if (!p) return;
		// auto* block = AllocBlockBase::s_fromMem(p);
		// auto  dataSize = block->data().size();
		// if (dataSize < AX_SIZEOF(T)) { AX_ASSERT(false); }
		//
		// //call destructor
		// Int numObject = dataSize / AX_SIZEOF(T);
		// MemoryUtil::destructor(p, numObject);
		ax_free(p);
	}
};

template<class T> AX_INLINE
void ax_delete(T* p) {
	AxDelete::deleteObject<T>(p);
}

template<class T> AX_INLINE
void ax_delete_array(T* p) {
	AxDelete::deleteArray<T>(p);
}

template<class T> AX_INLINE void ax_no_delete(T*) {}

void* ax_operator_new_handler(size_t sizeInBytes, const MemAllocRequest& req_);

void ax_operator_delete_handler(void* block, const MemAllocRequest& req);

} // namespace

//----- Global namespace
export AX_INLINE void* operator new  (size_t sizeInBytes, const ::ax::MemAllocRequest& req) { return ::ax::ax_operator_new_handler(sizeInBytes, req); }
export AX_INLINE void* operator new[](size_t sizeInBytes, const ::ax::MemAllocRequest& req) { return ::ax::ax_operator_new_handler(sizeInBytes, req); }

// get called, if constructor throw exception during 'new'
export AX_INLINE void operator delete  (void* p, const ::ax::MemAllocRequest& req) { ::ax::ax_operator_delete_handler(p, req); }
export AX_INLINE void operator delete[](void* p, const ::ax::MemAllocRequest& req) { ::ax::ax_operator_delete_handler(p, req); }

