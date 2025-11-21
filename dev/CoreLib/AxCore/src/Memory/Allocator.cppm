export module AxCore.Allocator;

import <cstdlib>;
import <format>;
import <iostream>;

import AxCore.Math;

#include "AxBase.h"
export import AxCore.BasicType;

export namespace ax {

template<class T>
class MemoryBlock : public NonCopyable {
public:
	class Allocator* allocator = nullptr;
	T*  data	= nullptr;
	Int size	= 0;

	constexpr MemoryBlock() = default;
	constexpr MemoryBlock(const MemoryBlock&) = delete;

	constexpr MemoryBlock(MemoryBlock&& r) noexcept
		: data(r.data)
		, allocator(r.allocator) {
		r.data = nullptr;
		r.size = 0;
	}

	constexpr MemoryBlock(Allocator* allocator_, T* data_, Int size_) noexcept
		: allocator(allocator_)
		, data(data_)
		, size(size_) {
	}
	
	constexpr ~MemoryBlock() noexcept { dealloc(); }
	
	constexpr void detach() noexcept { data = nullptr; allocator = nullptr; size = 0; }
	constexpr void dealloc();
};

class Allocator {
public:

	template<class T>
	MemoryBlock<T> alloc(Int reqSize, Int alignment = ax_alignof<T>) {
		constexpr Int kMinByteSize = 64;
		reqSize = Math::max(reqSize, kMinByteSize / ax_sizeof<T>);
		reqSize = Math::nextPow2_half(reqSize);
		
		auto block = onAlloc(reqSize * ax_sizeof<T>, alignment);
		T* data = reinterpret_cast<T*>(block.data);
		block.detach();
		return MemoryBlock<T>(this, data, reqSize);
	}
	void dealloc(void* p) { return onDealloc(p); }
	
protected:
	virtual MemoryBlock<u8> onAlloc(Int size, Int alignment) {
		void* data = ::_aligned_malloc(size, alignment);
		std::cout << std::format("onAlloc size={} data={}\n", size, data);
		return MemoryBlock<u8>(this, reinterpret_cast<u8*>(data), size);
	}

	virtual void onDealloc(void* data) {
		if (!data) return;
		std::cout << std::format("onDealloc data={}\n", data);
		::_aligned_free(data);
	}
};

template<class T> inline
constexpr void MemoryBlock<T>::dealloc() {
	if (!data || !allocator) return;
	allocator->dealloc(data);
	data = nullptr;
	size = 0;
}

Allocator*	ax_default_allocator();

} // namespace
