export module AxCore.Allocator;

import <cstdlib>;

#include "AxBase.h"
export import AxCore.BasicType;

export namespace ax {

class Allocator;

class MemoryBlock : public NonCopyable {
	friend class Allocator;
protected:
	void* _data	= nullptr;
	Allocator* _allocator = nullptr;
public:
	MemoryBlock() = default;
	MemoryBlock(const MemoryBlock&) = delete;
	MemoryBlock(MemoryBlock&& r) : _data(r._data), _allocator(r._allocator) {
		r._data = nullptr;
	} 
	~MemoryBlock() { dealloc(); }

	void detach() { _data = nullptr; _allocator = nullptr; }
	
	void* data() const { return _data; }
	
	void alloc(Int size) {
		dealloc();
		_data = ::malloc(size);
	}
	void dealloc();
};

class Allocator {
public:

	MemoryBlock alloc(Int size, Int alignment) { return onAlloc(size, alignment); }
	void dealloc(MemoryBlock & block) { return onDealloc(block); }
	
protected:
	virtual MemoryBlock onAlloc(Int size, Int alignment) {
		MemoryBlock block;
		block._allocator = this;
		block._data = ::_aligned_malloc(size, alignment);
		return block;
	}

	virtual void onDealloc(MemoryBlock & block) {
		if (!block._data) return;
		
		::free(block._data);
		block._data = nullptr;
	}
};

void MemoryBlock::dealloc() {
	if (!_data) return;
	if (!_allocator) throw Error_Allocator(AX_SRC_LOC);
	_allocator->dealloc(*this);
}

Allocator*	ax_default_allocator();

} // namespace
