export module AxCore.Allocator;

import <cstdlib>;
import <format>;
import <iostream>;

#include "AxBase.h"
export import AxCore.BasicType;

export namespace ax {

template<class T>
class MemoryBlock : public NonCopyable {
	friend class Allocator;
protected:
	Allocator* _allocator = nullptr;
	T*  _data	= nullptr;
	Int _size	= 0;
public:
	constexpr MemoryBlock() = default;
	constexpr MemoryBlock(const MemoryBlock&) = delete;
	constexpr MemoryBlock(MemoryBlock&& r) noexcept : _data(r._data), _allocator(r._allocator) {
		r._data = nullptr;
	}
	constexpr MemoryBlock(Allocator* allocator, T* data, Int size ) noexcept : _allocator(allocator), _data(data), _size(size) {}
	constexpr ~MemoryBlock() noexcept { dealloc(); }

	constexpr T* data() const noexcept { return _data; }
	constexpr Int size() const noexcept { return _size; }
	constexpr Allocator* allocator() const noexcept { return _allocator; }
	
	constexpr void detach() noexcept { _data = nullptr; _allocator = nullptr; _size = 0; }
	constexpr void dealloc();
};

class Allocator {
public:

	template<class T>
	MemoryBlock<T> alloc(Int size, Int alignment = AX_ALIGN_OF(T)) {
		auto block = onAlloc(size * AX_SIZE_OF(T), alignment);
		T* data = reinterpret_cast<T*>(block.data());
		block.detach();
		return MemoryBlock<T>(this, data, size);
	}
	void dealloc(void* p) { return onDealloc(p); }
	
protected:
	virtual MemoryBlock<u8> onAlloc(Int size, Int alignment) {
		void* data = ::_aligned_malloc(size, alignment);
		// std::cout << std::format("onAlloc size={} data={}\n", size, data);
		return MemoryBlock<u8>(this, reinterpret_cast<u8*>(data), size);
	}

	virtual void onDealloc(void* data) {
		if (!data) return;
		// std::cout << std::format("onDealloc data={}\n", data);
		::_aligned_free(data);
	}
};

template<class T> inline
constexpr void MemoryBlock<T>::dealloc() {
	if (!_data) return;
	if (!_allocator) throw Error_Allocator();
	_allocator->dealloc(_data);
	_data = nullptr;
	_size = 0;
}

Allocator*	ax_default_allocator();

} // namespace
